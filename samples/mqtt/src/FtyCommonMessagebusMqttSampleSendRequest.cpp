/*  =========================================================================
    FtyCommonMessagebusMqttSampleSendRequest - description

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
    FtyCommonMessagebusMqttSampleSendRequest -
@discuss
@end
*/

#include "fty/messagebus/test/FtyCommonMathDto.hpp"
#include <fty/messagebus/MsgBusMqtt.hpp>

#include <csignal>
#include <fty_log.h>
#include <iostream>

namespace
{
  using namespace fty::messagebus;
  using namespace fty::messagebus::test;
  using Message = fty::messagebus::mqttv5::MqttMessage;

  static bool _continue = true;
  static auto constexpr SYNC_REQUEST_TIMEOUT = 5;

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
  if (argc != 6)
  {
    std::cout << "USAGE: " << argv[0] << " <reqQueue> <async|sync> <add|mult> <num1> <num2>" << std::endl;
    return EXIT_FAILURE;
  }

  log_info("%s - starting...", argv[0]);

  auto requestQueue = std::string{argv[1]};

  // Install a signal handler
  std::signal(SIGINT, signalHandler);
  std::signal(SIGTERM, signalHandler);

  auto reqRep = MsgBusMqtt();

  auto query = MathOperation(argv[3], std::stoi(argv[4]), std::stoi(argv[5]));

  if (strcmp(argv[2], "async") == 0)
  {
    reqRep.sendRequest(requestQueue, query.serialize(), responseMessageListener);
  }
  else
  {
    _continue = false;

    Opt<Message> replyMsg = reqRep.sendRequest(requestQueue, query.serialize(), SYNC_REQUEST_TIMEOUT);
    if (replyMsg.has_value())
    {
      responseMessageListener(replyMsg.value());
    }
    else
    {
      log_error("Time out reached: (%ds)", SYNC_REQUEST_TIMEOUT);
    }
  }

  while (_continue)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  log_info("%s - end", argv[0]);
  return EXIT_SUCCESS;
}
