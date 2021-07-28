/*  =========================================================================
    FtyCommonMessagebusMlmSampleRep.cpp - Provides message bus for agents

    Copyright (C) 2019 - 2020 Eaton

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

  auto receiver = fty::messagebus::MsgBusMalamute();
  static bool _continue = true;

  static void signalHandler(int signal)
  {
    std::cout << "Signal " << signal << " received\n";
    _continue = false;
  }

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
      FooBar fooBarr = FooBar("status::ok", fooBar.bar.c_str());
      UserData userData;
      userData << fooBarr;
      receiver.sendRequestReply(message, userData);
    }
    else
    {
      log_info("Old format, skip query...");
    }
  }
} // namespace

int main(int /*argc*/, char** argv)
{
  log_info("%s - starting...", argv[0]);

  std::signal(SIGINT, signalHandler);
  std::signal(SIGTERM, signalHandler);

  receiver.registerRequestListener("doAction.queue.query", queryListener);

  while (_continue)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  log_info("%s - end", argv[0]);

  return EXIT_SUCCESS;
}
