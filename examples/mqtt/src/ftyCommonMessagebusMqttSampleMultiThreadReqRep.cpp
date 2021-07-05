/*  =========================================================================
    ftyCommonMessagebusMqttSampleRep.cpp - description

    Copyright (C) 2014 - 2021 Eaton

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
    =========================================================================
*/

/*
@header
    ftyCommonMessagebusMqttSampleMultithred.cpp -
@discuss
@end
*/
#include "FtyCommonMqttTestDef.hpp"
#include <FtyCommonMqttTestMathDto.h>
#include <fty/messagebus/MsgBusException.hpp>
#include <fty/messagebus/MsgBusFactory.hpp>
#include <fty/messagebus/utils/MsgBusHelper.hpp>

#include <mqtt/async_client.h>

#include <cctype>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <fty_log.h>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <thread>

namespace
{
  using namespace fty::messagebus;
  using namespace fty::messagebus::mqttv5;
  using namespace fty::messagebus::mqttv5::test;
  using namespace fty::messagebus::test;
  using Message = fty::messagebus::mqttv5::MqttMessage;
  using MessageBus = fty::messagebus::IMessageBus<Message>;

  std::unique_ptr<MessageBus> mqttMsgBus;

  static bool _continue = true;
  static auto correlationIdSniffer = std::map<std::string, std::string>();

  auto getClientName() -> std::string
  {
    return utils::getClientId("MqttSampleStress");
  }

  static void signalHandler(int signal)
  {
    std::cout << "Signal " << signal << " received\n";
    _continue = false;
  }

  static auto buildRandom(int min, int max) -> int
  {
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni(min, max);
    return uni(rng);
  }

  void mathOperationListener(const Message& message)
  {
    log_info("Question arrived");
    Message response;

    auto mathQuery = MathOperation(message.userData());
    auto mathResult = MathResult();

    if (mathQuery.operation == "add")
    {
      mathResult.result = mathQuery.param_1 + mathQuery.param_2;
      response.metaData().emplace(STATUS, STATUS_OK);
    }
    else if (mathQuery.operation == "mult")
    {
      mathResult.result = mathQuery.param_1 * mathQuery.param_2;
      response.metaData().emplace(STATUS, STATUS_OK);
    }
    else
    {
      mathResult.status = MathResult::STATUS_KO;
      mathResult.error = "Unsuported operation";
      response.metaData().emplace(STATUS, STATUS_KO);
    }

    response.userData() = mathResult.serialize();
    response.metaData().emplace(SUBJECT, ANSWER_USER_PROPERTY);
    response.metaData().emplace(FROM, message.metaData().find(FROM)->second);
    response.metaData().emplace(CORRELATION_ID, message.metaData().find(CORRELATION_ID)->second);
    response.metaData().emplace(REPLY_TO, message.metaData().find(REPLY_TO)->second);

    mqttMsgBus->sendReply(message.metaData().find(REPLY_TO)->second, response);

    //_continue = false;
  }

  void responseListener(const Message& message)
  {
    log_info("Answer arrived");
    auto mathresult = MathResult(message.userData());
    log_info("  * status: '%s', result: %d, error: '%s'", mathresult.status.c_str(), mathresult.result, mathresult.error.c_str());

    auto iterator = message.metaData().find(CORRELATION_ID);
    if (iterator == message.metaData().end() || iterator->second == "")
    {
      throw MessageBusException("Reply error not correlationId");
    }
    auto correlationId = iterator->second;
    log_info("correlationId responseListener %s size: %d", correlationId.c_str(), correlationIdSniffer.size());

    iterator = message.metaData().find(FROM);
    if (iterator == message.metaData().end() || iterator->second == "")
    {
      throw MessageBusException("Reply error not from");
    }
    auto rand = iterator->second;

    iterator = correlationIdSniffer.find(correlationId);
    if (iterator == correlationIdSniffer.end() || iterator->second == "")
    {
      throw MessageBusException("Error on correlationIdSniffer");
    }
    if (iterator->second == rand)
    {
      log_info("The answer is correct");
      correlationIdSniffer.erase(iterator);
    }
    else
    {
      throw MessageBusException("Reply error the answer is not correlated to the id");
    }
  }

  void requesterFunc(std::unique_ptr<MessageBus> messageBus)
  {
    auto correlationId = utils::generateUuid();
    auto replyTo = REPLY_QUEUE + '/' + correlationId;

    auto rand = buildRandom(1, 10);

    Message message;
    message.userData() = MathOperation("add", 1, rand).serialize();
    message.metaData().clear();
    message.metaData().emplace(SUBJECT, QUERY_USER_PROPERTY);
    message.metaData().emplace(FROM, std::to_string(rand));
    message.metaData().emplace(REPLY_TO, replyTo);
    message.metaData().emplace(CORRELATION_ID, correlationId);


    correlationIdSniffer.emplace(correlationId, std::to_string(rand));
    messageBus->receive(replyTo, responseListener);

log_info("correlationId requesterFunc %s size: %d", correlationId.c_str(), correlationIdSniffer.size());
    //replyerFunc(mqttMsgBus);
    // mqttMsgBus->receive(messagebus::REQUEST_QUEUE, mathOperationListener);
    // mqttMsgBus->sendRequest(messagebus::REQUEST_QUEUE, message);
    messageBus->sendRequest(REQUEST_QUEUE, message, mathOperationListener);
  }

} // namespace

int main(int /*argc*/, char** argv)
{
  log_info("%s - starting...", argv[0]);

  // Install a signal handler
  std::signal(SIGINT, signalHandler);
  std::signal(SIGTERM, signalHandler);

  mqttMsgBus = MessageBusFactory::createMqttMsgBus(DEFAULT_MQTT_END_POINT, getClientName());
  mqttMsgBus->connect();

  requesterFunc(std::move(mqttMsgBus));

  //replyerFunc(mqttMsgBus2);

  //std::thread replyer(replyerFunc, mqttMsgBus);
  //std::thread requester(requesterFunc, mqttMsgBus);

  while (_continue)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Close the counter and wait for the publisher thread to complete
  log_info("Shutting down...");
  log_info("Sniffer count %d", correlationIdSniffer.size());
  //requester.join();
  //replyer.join();

  // delete mqttMsgBus;

  log_info("%s - end", argv[0]);
  return EXIT_SUCCESS;
}
