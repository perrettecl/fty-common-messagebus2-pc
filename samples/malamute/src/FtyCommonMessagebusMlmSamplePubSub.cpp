/*  =========================================================================
    FtyCommonMessagebusMlmSamplePubSub - description

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
    FtyCommonMessagebusMlmSamplePubSub -
@discuss
@end
*/

#include <fty/messagebus/test/FtyCommonTestDef.hpp>

#include <fty/messagebus/MsgBusMalamute.hpp>
#include <fty/messagebus/test/FtyCommonFooBarDto.hpp>

#include <csignal>
#include <fty_log.h>
#include <iostream>

namespace
{
  using namespace fty::messagebus::test;
  using Message = fty::messagebus::mlm::MlmMessage;

  static bool _continue = true;

  static void signalHandler(int signal)
  {
    std::cout << "Signal " << signal << " received\n";
    _continue = false;
  }

  void messageListener(Message message)
  {
    log_info("messageListener");
    for (const auto& pair : message.metaData())
    {
      log_info("  ** '%s' : '%s'", pair.first.c_str(), pair.second.c_str());
    }

    FooBar fooBar;
    message.userData() >> fooBar;
    log_info("  * foo    : '%s'", fooBar.foo.c_str());
    log_info("  * bar    : '%s'", fooBar.bar.c_str());

    _continue = false;
  }
} // namespace

int main(int /*argc*/, char** argv)
{
  log_info("%s - starting...", argv[0]);

  // Install a signal handler
  std::signal(SIGINT, signalHandler);
  std::signal(SIGTERM, signalHandler);

  auto pubSub = fty::messagebus::MsgBusMalamute();
  pubSub.subscribe(SAMPLE_TOPIC, messageListener);

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  auto fooBar = FooBar("event", "hello");
  UserData userData;
  userData << fooBar;
  pubSub.publish(SAMPLE_TOPIC, userData);

  while (_continue)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  log_info("%s - end", argv[0]);
  return EXIT_SUCCESS;
}
