/*  =========================================================================
    FtyCommonMessagebusMlmSample - description

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

#include <fty/messagebus/test/FtyCommonTestDef.hpp>

#include <fty/messagebus/MsgBusMalamute.hpp>
#include <fty/messagebus/test/FtyCommonFooBarDto.hpp>

#include <fty_log.h>
#include <thread>

namespace
{
  using namespace fty::messagebus;
  using namespace fty::messagebus::test;
  using Message = fty::messagebus::mlm::MlmMessage;

  auto receiver = fty::messagebus::MsgBusMalamute();
  auto publisher = fty::messagebus::MsgBusMalamute();

  void messageListener(Message message)
  {
    log_info("messageListener:");
    auto metadata = message.metaData();
    for (const auto& pair : message.metaData())
    {
      log_info("  ** '%s' : '%s'", pair.first.c_str(), pair.second.c_str());
    }
    auto data = message.userData();
    FooBar fooBar;
    data >> fooBar;
    log_info("  * foo    : '%s'", fooBar.foo.c_str());
    log_info("  * bar    : '%s'", fooBar.bar.c_str());
  }

  void queryListener(Message message)
  {
    log_info("queryListener:");
    for (const auto& pair : message.metaData())
    {
      log_info("  ** '%s' : '%s'", pair.first.c_str(), pair.second.c_str());
    }
    auto data = message.userData();
    FooBar fooBar;
    data >> fooBar;
    log_info("  * foo    : '%s'", fooBar.foo.c_str());
    log_info("  * bar    : '%s'", fooBar.bar.c_str());

    if (message.metaData().size() != 0)
    {
      Message response;
      MetaData metadata;
      auto fooBarr = FooBar("status", "ok");
      UserData data2;
      data2 << fooBarr;
      response.userData() = data2;
      response.metaData().emplace(SUBJECT, "response");
      response.metaData().emplace(TO, message.metaData().find(FROM)->second);
      response.metaData().emplace(CORRELATION_ID, message.metaData().find(CORRELATION_ID)->second);
      if (fooBar.bar == "wait")
      {
        std::this_thread::sleep_for(std::chrono::seconds(10));
      }
      publisher->sendReply(message.metaData().find(REPLY_TO)->second, response);
    }
    else
    {
      log_info("Old format, skip query...");
    }
  }

  void responseListener(const Message& message)
  {
    log_info("responseListener:");
    for (const auto& pair : message.metaData())
    {
      log_info("  ** '%s' : '%s'", pair.first.c_str(), pair.second.c_str());
    }
    auto data = message.userData();
    FooBar fooBar;
    data >> fooBar;
    log_info("  * foo    : '%s'", fooBar.foo.c_str());
    log_info("  * bar    : '%s'", fooBar.bar.c_str());
  }

  void responseListener2(const Message& message)
  {
    log_info("Specific responseListener:");
    for (const auto& pair : message.metaData())
    {
      log_info("  ** '%s' : '%s'", pair.first.c_str(), pair.second.c_str());
    }
    auto data = message.userData();
    FooBar fooBar;
    data >> fooBar;
    log_info("  * foo    : '%s'", fooBar.foo.c_str());
    log_info("  * bar    : '%s'", fooBar.bar.c_str());
  }
} // namespace

int main(int /*argc*/, char** argv)
{
  log_info("%s - starting...", argv[0]);

  // receiver = MessageBusFactory::createMlmMsgBus(DEFAULT_MLM_END_POINT, "receiver");
  // receiver->connect();
  receiver.subscribe("discovery", messageListener);
  receiver.waitRequest("doAction.queue.query", queryListener);
  // old mailbox mecanism
  receiver.waitRequest("receiver", queryListener);

  // publisher = MessageBusFactory::createMlmMsgBus(DEFAULT_MLM_END_POINT, "publisher");
  // publisher->connect();
  publisher-waitRequest("doAction.queue.response", responseListener);
  std::this_thread::sleep_for(std::chrono::seconds(2));

  // PUBLISH
  //Message message;
  auto hello = FooBar("event", "hello");
  UserData userdata;
  userData << hello;
  //message.userData() << hello;
  // message.metaData().clear();
  // message.metaData().emplace("mykey", "myvalue");
  // message.metaData().emplace(FROM, "publisher");
  // message.metaData().emplace(SUBJECT, "discovery");

  publisher.publish("discovery", userData);
  std::this_thread::sleep_for(std::chrono::seconds(5));

  // PUBLISH EXCEPTION
  try
  {
    publisher.publish("discovery2", userData);
  }
  catch (MessageBusException& ex)
  {
    log_error("%s", ex.what());
  }

  // REQUEST
  Message message2;
  auto query1 = FooBar("doAction", "actionNothing");
  UserData userdataQuery1;
  userdataQuery1 << query1;
  // message2.userData() << query1;
  // message2.metaData().clear();
  // message2.metaData().emplace(CORRELATION_ID, utils::generateUuid());
  // message2.metaData().emplace(SUBJECT, "doAction");
  // message2.metaData().emplace(FROM, "publisher");
  // message2.metaData().emplace(TO, "receiver");
  // message2.metaData().emplace(REPLY_TO, "doAction.queue.response");
  publisher.sendRequest("doAction.queue.query", query1);
  std::this_thread::sleep_for(std::chrono::seconds(5));

  // REQUEST 2
  Message message6;
  FooBar query4 = FooBar("doAction", "actionNothing2");
  message6.userData() << query4;
  message6.metaData().clear();
  message6.metaData().emplace(CORRELATION_ID, utils::generateUuid());
  message6.metaData().emplace(SUBJECT, "doAction");
  message6.metaData().emplace(FROM, "publisher");
  message6.metaData().emplace(TO, "receiver");
  message6.metaData().emplace(REPLY_TO, "doAction.queue.response2");
  publisher->sendRequest("doAction.queue.query", message6, responseListener2);
  std::this_thread::sleep_for(std::chrono::seconds(5));

  // REQUEST WITHOUT METADATA
  Message message3;
  auto query2 = FooBar("doAction2", "actionNothing");
  message3.userData() << query2;
  message3.metaData().clear();
  publisher->sendRequest("receiver", message3);
  std::this_thread::sleep_for(std::chrono::seconds(5));

  // SYNC REQUEST
  Message message5;
  auto query3 = FooBar("doAction3", "wait");
  message5.userData() << query3;
  message5.metaData().clear();
  message5.metaData().emplace(SUBJECT, "sync query");
  message5.metaData().emplace(FROM, "publisher");
  message5.metaData().emplace(TO, "receiver");
  try
  {
    publisher->request("doAction.queue.query", message5, 15);
  }
  catch (MessageBusException& ex)
  {
    log_error("%s", ex.what());
  }
  message5.metaData().emplace(CORRELATION_ID, utils::generateUuid());
  auto resp = publisher->request("doAction.queue.query", message5, 15);
  if (resp.has_value())
  {
    log_info("Response sync:");
    for (const auto& pair : resp.value().metaData())
    {
      log_info("  ** '%s' : '%s'", pair.first.c_str(), pair.second.c_str());
    }
    auto data = resp.value().userData();
    FooBar fooBar;
    data >> fooBar;
    log_info("  * foo    : '%s'", fooBar.foo.c_str());
    log_info("  * bar    : '%s'", fooBar.bar.c_str());
  }
  else
  {
    log_info("Timeout reached");
  }

  // PUBLISH WITHOUT METADATA
  Message message4;
  auto bye = FooBar("event", "bye");
  message4.userData() << bye;
  message4.metaData().clear();
  publisher->publish("discovery", message4);
  std::this_thread::sleep_for(std::chrono::seconds(5));

  log_info(argv[0]);
  return EXIT_SUCCESS;
}
