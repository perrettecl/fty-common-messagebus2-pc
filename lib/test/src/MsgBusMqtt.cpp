/*  =========================================================================
    MsgBusMqtt.cpp - description

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

#define UNIT_TESTS

#include <fty/messagebus/MsgBusMqtt.hpp>

#include <catch2/catch.hpp>
#include <iostream>

// NOTE: This test case requires network access. It uses one of
// the public available MQTT brokers
#if defined(TEST_EXTERNAL_SERVER)
static const std::string MQTT_SERVER_URI{"tcp://mqtt.eclipse.org:1883"};
#else
static const std::string MQTT_SERVER_URI{"tcp://localhost:1883"};
#endif

static int MAX_TIMEOUT = 1;
static constexpr auto TEST_QUEUE = "/queueTest";
static constexpr auto TEST_TOPIC = "/topicTest";
static const std::string QUERY = "query";
static const std::string QUERY_2 = "query2";
static const std::string OK = ":OK";
static const std::string RESPONSE = QUERY + OK;
static const std::string RESPONSE_2 = QUERY_2 + OK;

namespace
{
  using namespace fty::messagebus;
  using Message = fty::messagebus::mqttv5::MqttMessage;

  static auto s_msgBus = MsgBusMqtt("TestCase", MQTT_SERVER_URI);

  // Response listener
  void replyerListener(const Message& message)
  {
    s_msgBus.sendRequestReply(message, message.userData() + OK);
  }

  // Replyer listener
  void responseListener(Message message)
  {
    assert(message.userData() == RESPONSE);
  }

  //----------------------------------------------------------------------
  // Test case
  //----------------------------------------------------------------------

  TEST_CASE("Mqtt identify implementation", "[identify]")
  {
    std::size_t found = s_msgBus.identify().find("MQTT");
    REQUIRE(found != std::string::npos);
  }

  TEST_CASE("Mqtt sync request", "[sendRequest]")
  {
    auto msgBus = MsgBusMqtt("MqttSyncRequestTestCase", MQTT_SERVER_URI);

    DeliveryState state = msgBus.registerRequestListener(TEST_QUEUE, replyerListener);
    REQUIRE(state == DeliveryState::DELI_STATE_ACCEPTED);

    // Send synchronous request
    Opt<Message> replyMsg = msgBus.sendRequest(TEST_QUEUE, QUERY, MAX_TIMEOUT);
    REQUIRE(replyMsg.has_value());
    REQUIRE(replyMsg.value().userData() == RESPONSE);

    replyMsg = msgBus.sendRequest(TEST_QUEUE, QUERY_2, MAX_TIMEOUT);
    REQUIRE(replyMsg.has_value());
    REQUIRE(replyMsg.value().userData() == RESPONSE_2);
  }


  TEST_CASE("Mqtt async request", "[sendRequest]")
  {
    auto msgBus = MsgBusMqtt("MqttAsyncRequestTestCase", MQTT_SERVER_URI);

    DeliveryState state = msgBus.registerRequestListener(TEST_QUEUE, replyerListener);
    REQUIRE(state == DeliveryState::DELI_STATE_ACCEPTED);

    state = msgBus.sendRequest(TEST_QUEUE, QUERY, responseListener);
    REQUIRE(state == DeliveryState::DELI_STATE_ACCEPTED);
    // Wait to process the response
    std::this_thread::sleep_for(std::chrono::seconds(MAX_TIMEOUT));
  }

  TEST_CASE("Mqtt publish subscribe", "[publish]")
  {
    auto msgBus = MsgBusMqtt("MqttPubSubTestCase", MQTT_SERVER_URI);

    DeliveryState state = msgBus.subscribe(TEST_TOPIC, responseListener);
    REQUIRE(state == DeliveryState::DELI_STATE_ACCEPTED);

    state = msgBus.publish(TEST_TOPIC, RESPONSE);
    REQUIRE(state == DeliveryState::DELI_STATE_ACCEPTED);
    // Wait to process publish
    std::this_thread::sleep_for(std::chrono::seconds(MAX_TIMEOUT));
  }
} // namespace
