/*  =========================================================================
    FtyCommonMessagebusMlmSampleRequestReply - description

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
    FtyCommonMessagebusMlmSampleRequestReply -
@discuss
@end
*/

#include "fty/messagebus/mlm/test/FtyCommonMlmTestDef.hpp"
#include <fty/messagebus/MsgBusMalamute.hpp>
#include <fty/messagebus/test/FtyCommonFooBarDto.hpp>
#include <fty/messagebus/test/FtyCommonTestDef.hpp>

#include <csignal>
#include <fty_log.h>
#include <iostream>
#include <thread>

namespace
{
  using namespace fty::messagebus;
  using namespace fty::messagebus::test;
  using Message = fty::messagebus::mlm::MlmMessage;

  auto answer = fty::messagebus::MsgBusMalamute("answer");
  auto requester = fty::messagebus::MsgBusMalamute("requester");

  void queryListener(const Message& message)
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
      auto fooBarr = FooBar("status", "ok");
      UserData userData;
      userData << fooBarr;
      if (fooBar.bar == "wait")
      {
        std::this_thread::sleep_for(std::chrono::seconds(1));
      }
      answer.sendRequestReply(message, userData);
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
    UserData data = message.userData();
    FooBar fooBar;
    data >> fooBar;
    log_info("  * foo    : '%s'", fooBar.foo.c_str());
    log_info("  * bar    : '%s'", fooBar.bar.c_str());
  }
} // namespace

int main(int /*argc*/, char** argv)
{
  log_info("%s - starting...", argv[0]);

  answer.registerRequestListener(fty::messagebus::mlm::test::QUEUE_NAME, queryListener);
  std::this_thread::sleep_for(std::chrono::seconds(2));

  // REQUEST
  Message message;
  auto query1 = FooBar("doAction", "wait");
  UserData userData;
  userData << query1;
  requester.sendRequest(answer.clientName(), fty::messagebus::mlm::test::QUEUE_NAME, userData, responseListener);
  std::this_thread::sleep_for(std::chrono::seconds(2));

  // REQUEST 2
  Message message2;
  auto query2 = FooBar("doAction again", "wait");
  UserData userData2;
  userData2 << query2;
  requester.sendRequest(answer.clientName(), fty::messagebus::mlm::test::QUEUE_NAME, userData2, responseListener);
  std::this_thread::sleep_for(std::chrono::seconds(5));

  log_info("%s - end", argv[0]);

  return EXIT_SUCCESS;
}
