/*  =========================================================================
    FtyCommonMessagebusMlmSampleReq.cpp - Provides message bus for agents

    Copyright (C) 2019 - 2021 Eaton

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

#include "fty/messagebus/mlm/test/FtyCommonMlmTestDef.hpp"
#include <fty/messagebus/MsgBusMalamute.hpp>
#include <fty/messagebus/test/FtyCommonFooBarDto.hpp>
#include <fty/messagebus/test/FtyCommonTestDef.hpp>

#include <csignal>
#include <fty_log.h>
#include <iostream>

namespace
{
  using namespace fty::messagebus;
  using namespace fty::messagebus::test;
  using Message = fty::messagebus::mlm::MlmMessage;

  auto requester = fty::messagebus::MsgBusMalamute();
  static bool _continue = true;

  static void signalHandler(int signal)
  {
    std::cout << "Signal " << signal << " received\n";
    _continue = false;
  }

} // namespace

int main(int argc, char** argv)
{
  int total = 100;
  log_info("%s - starting...", argv[0]);
  if (argc > 1)
  {
    log_info("%s", argv[1]);
    total = atoi(argv[1]);
  }

  std::signal(SIGINT, signalHandler);
  std::signal(SIGTERM, signalHandler);

  int count = 0;
  int rcv = 0;
  int loose = 0;
  do
  {
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
    std::string str(buffer);

    // SYNC REQUEST
    auto query = FooBar("doAction", std::to_string(count));
    UserData userData;
    userData << query;
    try
    {
      // Set the destination client name (specific to malamute!)
      requester.destClientName(fty::messagebus::mlm::test::SHARED_CLIENT_NAME);
      auto resp = requester.sendRequest(fty::messagebus::mlm::test::QUEUE_NAME, userData, 5);
      if (resp.has_value())
      {
        log_info("Response:");
        for (const auto& pair : resp.value().metaData())
        {
          log_info("  ** '%s' : '%s'", pair.first.c_str(), pair.second.c_str());
        }
        auto data = resp.value().userData();
        FooBar fooBar;
        data >> fooBar;
        log_info("  * foo    : '%s'", fooBar.foo.c_str());
        log_info("  * bar    : '%s'", fooBar.bar.c_str());
        rcv++;
      }
      else
      {
        log_info("Timeout reached");
        loose++;
      }
    }
    catch (MessageBusException& ex)
    {
      log_error("%s", ex.what());
      loose++;
    }
    count++;

  } while (_continue == true && (count < total));

  log_info("**************************************************");
  log_info(" total  : %d", count);
  log_info(" receive: %d", rcv);
  log_info(" loose  : %d", loose);
  log_info("**************************************************");

  log_info("%s - end", argv[0]);

  return EXIT_SUCCESS;
}
