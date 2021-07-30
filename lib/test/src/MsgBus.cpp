/*  =========================================================================
    MsgBus.cpp - description

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

#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_DISABLE_EXCEPTIONS

#include <fty/messagebus/MsgBusMqtt.hpp>
// #include <fty/messagebus/test/FtyCommonFooBarDto.hpp>

#include <catch2/catch.hpp>
#include <iostream>

TEST_CASE("Mqtt test")
{
  std::cout << " * Common message bus testing: " << std::endl;
  auto reqRep = fty::messagebus::MsgBusMqtt();

  /*Opt<Message>*/ auto replyMsg = reqRep.sendRequest("tests", "query.serialize()", 2);
  REQUIRE(!replyMsg.has_value());
}
