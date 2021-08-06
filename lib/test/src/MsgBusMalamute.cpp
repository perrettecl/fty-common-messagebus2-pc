/*  =========================================================================
    MsgBusMalamute.cpp - description

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

#include "fty/messagebus/test/MsgBusTestCommon.hpp"
#include <fty/messagebus/MsgBusMalamute.hpp>

#include <catch2/catch.hpp>
#include <iostream>

namespace
{
  static constexpr auto MALAMUTE_SERVER_URI{"ipc://@/malamute"};
  //static constexpr auto MALAMUTE_SERVER_URI{"inproc://fty_common_messagebus2-test"};

  static constexpr auto TEST_QUEUE = "testQueue";
  static constexpr auto TEST_TOPIC = "testTopic";

  using Catch::Matchers::Contains;

  using namespace fty::messagebus;
  using namespace fty::messagebus::test;
  using UserData = fty::messagebus::mlm::UserData;
  using Message = fty::messagebus::mlm::MlmMessage;

  static auto s_msgBus = MsgBusMalamute("TestCase", MALAMUTE_SERVER_URI);

  // Response listener
  void replyerListener(const Message& message)
  {
    UserData userData;
    userData.emplace_front(message.userData().front() + OK);
    s_msgBus.sendRequestReply(message, userData);
  }

  // Replyer listener
  void responseListener(Message message)
  {
    assert(message.userData().front() == RESPONSE);
  }

  //----------------------------------------------------------------------
  // Test case
  //----------------------------------------------------------------------

  TEST_CASE("Malamute identify implementation", "[identify]")
  {
    std::size_t found = s_msgBus.identify().find("Malamute");
    REQUIRE(found != std::string::npos);
  }

  TEST_CASE("Malamute sync request", "[sendRequest]")
  {
    auto msgBus = MsgBusMalamute("MalamuteSyncRequestTestCase", MALAMUTE_SERVER_URI, s_msgBus.clientName());

    DeliveryState state = s_msgBus.registerRequestListener(TEST_QUEUE, replyerListener);
    REQUIRE(state == DeliveryState::DELI_STATE_ACCEPTED);

    // Send synchronous request
    UserData userData;
    userData.emplace_front(QUERY);
    Opt<Message> replyMsg = msgBus.sendRequest(TEST_QUEUE, userData, MAX_TIMEOUT);
    REQUIRE(replyMsg.has_value());
    REQUIRE(replyMsg.value().userData().front() == RESPONSE);

    userData.clear();
    userData.emplace_front(QUERY_2);
    replyMsg = msgBus.sendRequest(TEST_QUEUE, userData, MAX_TIMEOUT);
    REQUIRE(replyMsg.has_value());
    REQUIRE(replyMsg.value().userData().front() == RESPONSE_2);
  }

  TEST_CASE("Malamute async request", "[sendRequest]")
  {
    auto msgBus = MsgBusMalamute("MalamuteASyncRequestTestCase", MALAMUTE_SERVER_URI, s_msgBus.clientName());

    DeliveryState state = s_msgBus.registerRequestListener(TEST_QUEUE, replyerListener);
    REQUIRE(state == DeliveryState::DELI_STATE_ACCEPTED);

    UserData userData;
    userData.emplace_front(QUERY);
    state = msgBus.sendRequest(TEST_QUEUE, userData, responseListener);
    REQUIRE(state == DeliveryState::DELI_STATE_ACCEPTED);
    // Wait to process the response
    std::this_thread::sleep_for(std::chrono::seconds(MAX_TIMEOUT));
  }

  TEST_CASE("Malamute async request with no malamute destination name set", "[sendRequest]")
  {
    auto msgBus = MsgBusMalamute("MalamuteSyncRequestMustThrow", MALAMUTE_SERVER_URI);

    REQUIRE_THROWS_WITH(msgBus.sendRequest(TEST_QUEUE, {}, responseListener), Contains( "destination name is not set" ) && Contains( "the request can not be send" ) );
  }

  TEST_CASE("Malamute publish subscribe", "[publish]")
  {
    auto msgBus = MsgBusMalamute("MalamutePubSubTestCase", MALAMUTE_SERVER_URI);

    DeliveryState state = s_msgBus.subscribe(TEST_TOPIC, responseListener);
    REQUIRE(state == DeliveryState::DELI_STATE_ACCEPTED);

    UserData userData;
    userData.emplace_front(RESPONSE);
    state = msgBus.publish(TEST_TOPIC, userData);
    REQUIRE(state == DeliveryState::DELI_STATE_ACCEPTED);
    // Wait to process publish
    std::this_thread::sleep_for(std::chrono::seconds(MAX_TIMEOUT));
  }

} // namespace
