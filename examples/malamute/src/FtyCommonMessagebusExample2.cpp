/*  =========================================================================
    fty_common_messagebus_example - description

    Copyright (C) 2014 - 2020 Eaton

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
    fty_common_messagebus_example -
@discuss
@end
*/
#include "FtyCommonMessageBusDto.hpp"
#include "fty_common_messagebus_Imessage.hpp"
#include "fty_common_messagebus_exception.h"
#include "fty_common_messagebus_factory.hpp"
#include "fty_common_messagebus_helper.hpp"

#include <fty_log.h>

namespace
{
  using namespace fty::messagebus;
  using namespace fty::messagebus::mlm;

  MessageBusMalamute* receiver;
  MessageBusMalamute* publisher;

  void queryListener(MlmMessage message)
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
      MlmMessage response;
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

  void responseListener(const MlmMessage& message)
  {
    log_info("responseListener:");
    for (const auto& pair : message.metaData())
    {
      log_info("  ** '%s' : '%s'", pair.first.c_str(), pair.second.c_str());
    }
    UserData data = message.userData();
    FooBar fooBar;
    data >> fooBar;
    log_info("  * foo    : '%s'", fooBar.foo.c_str());
    log_info("  * bar    : '%s'", fooBar.bar.c_str());
  }
} // namespace

int main(int /*argc*/, char** argv)
{
  log_info(argv[0]);

  receiver = MessagebusFactory::createMlmMsgBus(DEFAULT_MLM_END_POINT, "receiver");
  receiver->connect();

  publisher = MessagebusFactory::createMlmMsgBus(DEFAULT_MLM_END_POINT, "publisher");
  publisher->connect();

  receiver->receive("doAction.queue.query", queryListener);
  publisher->receive("doAction.queue.response", responseListener);
  std::this_thread::sleep_for(std::chrono::seconds(2));

  // REQUEST
  MlmMessage message;
  auto query1 = FooBar("doAction", "wait");
  message.userData() << query1;
  message.metaData().clear();
  message.metaData().emplace(CORRELATION_ID, helper::generateUuid());
  message.metaData().emplace(SUBJECT, "doAction");
  message.metaData().emplace(FROM, "publisher");
  message.metaData().emplace(TO, "receiver");
  message.metaData().emplace(REPLY_TO, "doAction.queue.response");
  publisher->sendRequest("doAction.queue.query", message);
  std::this_thread::sleep_for(std::chrono::seconds(2));

  // REQUEST 2
  MlmMessage message2;
  auto query2 = FooBar("doAction", "wait");
  message2.userData() << query2;
  message2.metaData().clear();
  message2.metaData().emplace(CORRELATION_ID, helper::generateUuid());
  message2.metaData().emplace(SUBJECT, "doAction");
  message2.metaData().emplace(FROM, "publisher");
  message2.metaData().emplace(TO, "receiver");
  message2.metaData().emplace(REPLY_TO, "doAction.queue.response");
  publisher->sendRequest("doAction.queue.query", message2);
  std::this_thread::sleep_for(std::chrono::seconds(15));

  delete publisher;
  delete receiver;

  log_info(argv[0]);
  return 0;
}
