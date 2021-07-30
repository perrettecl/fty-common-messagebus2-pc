/*  =========================================================================
    MsgBusAmqp.cpp - description

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

#include <fty/messagebus/MsgBusAmqp.hpp>

#include <catch2/catch.hpp>
#include <iostream>

// NOTE: This test case requires network access. It uses one of
// the public available Amqp brokers
#if defined(TEST_EXTERNAL_SERVER)
static const std::string AMQP_SERVER_URI{"??????"};
#else
static const std::string AMQP_SERVER_URI{"??????"};
#endif

namespace
{
  using namespace fty::messagebus;
  using Message = fty::messagebus::amqp::AmqpMessage;

  //----------------------------------------------------------------------
  // Test case
  //----------------------------------------------------------------------

  TEST_CASE("Amqp identify implementation", "[identify]")
  {
    REQUIRE(true);
  }

} // namespace
