/*  =========================================================================
    fty_common_messagebus_mqtt_example - description

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
    fty_common_messagebus_mqtt_example -
@discuss
@end
*/

#include "FtyCommonMqttTestDef.hpp"
#include "fty/messagebus/IMessage.hpp"
#include "FtyCommonMessageBusDto.hpp"
#include "fty/messagebus/MsgBusException.hpp"
#include "fty/messagebus/MsgBusFactory.hpp"
#include "fty/messagebus/utils/MsgBusHelper.hpp"

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
  using Message = fty::messagebus::mqttv5::MqttMessage;

  static bool _continue = true;

  static void signalHandler(int signal)
  {
    std::cout << "Signal " << signal << " received\n";
    _continue = false;
  }

  void messageListener(Message message)
  {
    log_info("messageListener");
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

    _continue = false;
  }
} // namespace

int main(int /*argc*/, char** argv)
{
  log_info("%s - starting...", argv[0]);

  // Install a signal handler
  std::signal(SIGINT, signalHandler);
  std::signal(SIGTERM, signalHandler);

  auto publisher = MessageBusFactory::createMqttMsgBus(DEFAULT_MQTT_END_POINT, utils::getClientId("MqttPublisher"));
  publisher->connect();

  auto subscriber = MessageBusFactory::createMqttMsgBus(DEFAULT_MQTT_END_POINT, utils::getClientId("MqttSubscriber"));
  subscriber->connect();
  subscriber->subscribe(SAMPLE_TOPIC, messageListener);

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  // Publish
  Message message;
  message.userData() << FooBar("event", "hello");
  message.metaData().clear();
  message.metaData().emplace("mykey", "myvalue");
  message.metaData().emplace(FROM, "publisher");
  message.metaData().emplace(SUBJECT, "discovery");
  publisher->publish(SAMPLE_TOPIC, message);

  while (_continue)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  delete publisher;
  delete subscriber;

  log_info("%s - end", argv[0]);
  return EXIT_SUCCESS;
}
