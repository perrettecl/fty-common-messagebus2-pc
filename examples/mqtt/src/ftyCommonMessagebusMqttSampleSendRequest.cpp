/*  =========================================================================
    ftyCommonMessagebusMqttSamplesReqRep - description

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
    ftyCommonMessagebusMqttSamplesReqRep -
@discuss
@end
*/

#include "FtyCommonMqttTestDef.hpp"
#include <FtyCommonMqttTestMathDto.h>
#include <fty/messagebus/MsgBusException.hpp>
#include <fty/messagebus/MsgBusFactory.hpp>
#include <fty/messagebus/mqtt/MsgBusMqtt.hpp>
#include <fty/messagebus/utils/MsgBusHelper.hpp>

#include <chrono>
#include <csignal>
#include <fty_log.h>
#include <iostream>
#include <thread>

namespace
{
  using namespace fty::messagebus;
  using namespace fty::messagebus::mqttv5;
  using namespace fty::messagebus::mqttv5::test;
  using namespace fty::messagebus::test;
  using Message = fty::messagebus::mqttv5::MqttMessage;

  static bool _continue = true;
  static auto constexpr WAIT_RESPONSE_FOR = 5;

  static void signalHandler(int signal)
  {
    std::cout << "Signal " << signal << " received\n";
    _continue = false;
  }

  void responseMessageListener(const Message& message)
  {
    log_info("Response arrived");
    auto mathresult = MathResult(message.userData());
    log_info("  * status: '%s', result: %d, error: '%s'", mathresult.status.c_str(), mathresult.result, mathresult.error.c_str());

    _continue = false;
  }

} // namespace

int main(int argc, char** argv)
{
  if (argc < 5)
  {
    std::cout << "USAGE: " << argv[0] << " <reqQueue> <async|sync> <add|mult> <num1> <num2>" << std::endl;
    return EXIT_FAILURE;
  }

  log_info("%s - starting...", argv[0]);

  auto requestQueue = std::string{argv[1]};

  // Install a signal handler
  std::signal(SIGINT, signalHandler);
  std::signal(SIGTERM, signalHandler);

  std::string clientName = utils::getClientId("MqttSampleMathRequester");

  auto requester = MessageBusFactory::createMqttMsgBus(DEFAULT_MQTT_END_POINT, clientName);
  requester->connect();

  auto correlationId = utils::generateUuid();
  auto replyTo = REPLY_QUEUE + '/' + correlationId;

  Message message;
  auto query = MathOperation(argv[3], std::stoi(argv[4]), std::stoi(argv[5]));
  message.userData() = query.serialize();
  message.metaData().clear();
  message.metaData().emplace(SUBJECT, QUERY_USER_PROPERTY);
  message.metaData().emplace(FROM, clientName);
  message.metaData().emplace(REPLY_TO, replyTo);
  message.metaData().emplace(CORRELATION_ID, correlationId);

  if (strcmp(argv[2], "async") == 0)
  {
    requester->receive(replyTo, responseMessageListener);
    requester->sendRequest(requestQueue, message);
  }
  else
  {
    _continue = false;
    try
    {
      auto replyMsg = requester->request(requestQueue, message, WAIT_RESPONSE_FOR);
      responseMessageListener(replyMsg);
    }
    catch (MessageBusException& ex)
    {
      log_error("Message bus error: %s", ex.what());
    }
    catch (std::exception& ex)
    {
      log_error("Unexpected error: %s", ex.what());
    }
  }

  while (_continue)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  delete requester;

  log_info("%s - end", argv[0]);
  return EXIT_SUCCESS;
}
