/*  =========================================================================
    FtyCommonMqttTestDef.hpp - class description

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

#ifndef FTY_COMMON_MQTT_TEST_DEF_HPP
#define FTY_COMMON_MQTT_TEST_DEF_HPP

#include <string>

namespace messagebus::mqttv5::test
{
  // Topic
  static auto constexpr SAMPLE_TOPIC{"/etn/t/metric/samplemqtt"};

  // Queues
  static auto constexpr REQUEST_QUEUE{"/etn/q/request/maths"};
  //static auto constexpr REPLY_QUEUE{"/etn/q/reply"};
  static const std::string REPLY_QUEUE = "/etn/q/reply/maths";

} // namespace messagebus

#endif // FTY_COMMON_MQTT_TEST_DEF_HPP
