/*  =========================================================================
    FtyCommonMessagebusMqttSampleStressLib.cpp - description

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
    FtyCommonMessagebusMqttSampleStressLib.cpp -
@discuss
@end
*/

#include "fty/messagebus/mqtt/test/FtyCommonMqttTestDef.hpp"

#include <fty/messagebus/MsgBusException.hpp>
#include <fty/messagebus/MsgBusFactory.hpp>
#include <fty/messagebus/test/FtyCommonMqttTestMathDto.hpp>
#include <fty/messagebus/utils/MsgBusHelper.hpp>

#include <mqtt/async_client.h>

#include <cctype>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <cstring>
#include <fty_log.h>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

namespace
{
  using namespace fty::messagebus;
  using namespace fty::messagebus::mqttv5;
  using namespace fty::messagebus::mqttv5::test;
  using namespace fty::messagebus::test;
  using Message = fty::messagebus::mqttv5::MqttMessage;
  using MessageBus = fty::messagebus::IMessageBus<Message>;

  std::unique_ptr<MessageBus> mqttMsgBus;
  static bool _continue = true;

  auto getClientName() -> std::string
  {
    return utils::getClientId("MqttSampleStress");
  }

  static void signalHandler(int signal)
  {
    std::cout << "Signal " << signal << " received\n";
    _continue = false;
  }

  /////////////////////////////////////////////////////////////////////////////

  /**
 * A thread-safe counter that can be used to occasionally signal a waiter on
 * every 10th increment.
 */
  class MultithrCounter
  {
    using guard = std::unique_lock<std::mutex>;

    size_t count;
    bool closed;
    mutable bool ready;
    mutable std::condition_variable cond;
    mutable std::mutex lock;

  public:
    // Declare a pointer type for sharing a counter between threads
    using ptr_t = std::shared_ptr<MultithrCounter>;

    // Create a new thread-safe counter with an initial count of zero.
    MultithrCounter()
      : count(0)
      , closed(false)
      , ready(false)
    {
    }

    // Determines if the counter has been closed.
    bool isClose() const
    {
      guard g(lock);
      return closed;
    }

    // Close the counter and signal all waiters.
    void close()
    {
      guard g(lock);
      closed = ready = true;
      cond.notify_all();
    }

    // Increments the count, and then signals once every 10 messages.
    void incr()
    {
      guard g(lock);
      if (closed)
      {
        throw std::string("Counter is closed");
      }
      if (++count % 10 == 0)
      {
        ready = true;
        g.unlock();
        cond.notify_all();
      }
    }

    // This will block the caller until at least 10 new messages received.
    size_t getCount() const
    {
      guard g(lock);
      cond.wait(g, [this] { return ready; });
      ready = false;
      return count;
    }
  };

  void messageListener(const Message& message)
  {
    log_info("Msg arrived: %s", message.userData().c_str());
  }

  // The MQTT publisher function will run in its own thread.
  // It runs until the receiver thread closes the counter object.
  void publisherFunc(std::unique_ptr<MessageBus> messageBus, MultithrCounter::ptr_t counter)
  {
    while (_continue)
    {
      size_t n = counter->getCount();
      if (counter->isClose() || n == 100)
      {
        _continue = false;
      }
      Message message;
      message.userData() = std::to_string(n);
      messageBus->publish(SAMPLE_TOPIC, message);
    }
  }

} // namespace

int main(int /*argc*/, char** argv)
{
  log_info("%s - starting...", argv[0]);

  // Install a signal handler
  std::signal(SIGINT, signalHandler);
  std::signal(SIGTERM, signalHandler);

  // Make a counter object also with a shared pointer.
  auto counter = std::make_shared<MultithrCounter>();

  mqttMsgBus = MessageBusFactory::createMqttMsgBus(DEFAULT_MQTT_END_POINT, getClientName());
  mqttMsgBus->connect();
  mqttMsgBus->subscribe(SAMPLE_TOPIC, messageListener);

  std::thread publisher(publisherFunc, std::move(mqttMsgBus), counter);

  while (_continue)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    counter->incr();
  }

  // Close the counter and wait for the publisher thread to complete
  log_info("Shutting down...");
  counter->close();
  publisher.join();

  log_info("%s - end", argv[0]);
  return EXIT_SUCCESS;
}
