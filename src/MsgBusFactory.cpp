/*  =========================================================================
    fty_common_messagebus_factory - class description

    Copyright (C) 2014 - 2020 Eaton

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
    fty_common_messagebus_factory -
@discuss
@end
*/

#include "MsgBusFactory.hpp"



namespace fty::messagebus
{

  auto MessagebusFactory::createMlmMsgBus(const std::string& _endpoint, const std::string& _clientName) -> mlm::MessageBusMalamute*
  {
    return new mlm::MessageBusMalamute(_endpoint, _clientName);
  }

  auto MessagebusFactory::createMqttMsgBus(const std::string& _endpoint, const std::string& _clientName) -> mqttv5::MessageBusMqtt*
  {
    return new mqttv5::MessageBusMqtt(_endpoint, _clientName);
  }

} // namespace messagebus
