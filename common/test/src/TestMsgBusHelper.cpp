/*  =========================================================================
    TestMsgBusHelper.cpp - class description

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

//#include <gtest/gtest.h>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include "fty/messagebus/utils/MsgBusHelper.hpp"


#include <iostream>
#include <set>

using namespace fty::messagebus;

TEST_CASE("MsgBusHelper")
{
    //auto uuid utils::generateUuid();
    std::string testString{"testString"};
    REQUIRE(testString == testString);

}

// TEST(cipherCipherProxy, TestCipher)
// {

//   ASSERT_EQ(testString, uuid);
// }
