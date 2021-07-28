/*  =========================================================================
    FtyCommonMessagebusMqttSampleAsyncReply.cpp - description

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
    FtyCommonMessagebusMqttSampleAsyncReply.cpp -
@discuss
@end
*/

#include <fty/messagebus/MsgBusMqtt.hpp>
#include <fty/messagebus/test/FtyCommonMathDto.hpp>
#include <fty/messagebus/test/FtyCommonTestDef.hpp>

#include <csignal>
#include <fty_log.h>
#include <iostream>

namespace
{
  using namespace fty::messagebus::test;
  using Message = fty::messagebus::mqttv5::MqttMessage;

  auto replyer = fty::messagebus::MsgBusMqtt();
  auto reqRep = fty::messagebus::MsgBusMqtt();
  static bool _continue = true;

  static void signalHandler(int signal)
  {
    std::cout << "Signal " << signal << " received\n";
    _continue = false;
  }

  void replyerMessageListener(const Message& message)
  {
    log_info("Replyer messageListener");

    for (const auto& pair : message.metaData())
    {
      log_info("  ** '%s' : '%s'", pair.first.c_str(), pair.second.c_str());
    }

    auto mathQuery = MathOperation(message.userData());
    auto mathResultResult = MathResult();

    if (mathQuery.operation == "add")
    {
      mathResultResult.result = mathQuery.param_1 + mathQuery.param_2;
    }
    else if (mathQuery.operation == "mult")
    {
      mathResultResult.result = mathQuery.param_1 * mathQuery.param_2;
    }
    else
    {
      mathResultResult.status = MathResult::STATUS_KO;
      mathResultResult.error = "Unsuported operation";
    }

    reqRep.sendRequestReply(message, mathResultResult.serialize());
    //_continue = false;
  }

} // namespace

int main(int /*argc*/, char** argv)
{
  log_info("%s - starting...", argv[0]);

  // Install a signal handler
  std::signal(SIGINT, signalHandler);
  std::signal(SIGTERM, signalHandler);
  reqRep.registerRequestListener(SAMPLE_QUEUE, replyerMessageListener);

  while (_continue)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  log_info("%s - end", argv[0]);
  return EXIT_SUCCESS;
}
