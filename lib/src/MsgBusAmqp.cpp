/*  =========================================================================
    MsgBusAmqp.hpp - class description

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

#include "fty/messagebus/MsgBusAmqp.hpp"

namespace fty::messagebus
{

  static constexpr auto AMQP_IMPL = "Amqp implementation";

  std::string MsgBusAmqp::identify() const
  {
    return AMQP_IMPL;
  }

  DeliveryState MsgBusAmqp::subscribe(const std::string& topic, MessageListener<AmqpMessage> messageListener)
  {
    return DeliveryState::DELI_STATE_UNAVAILABLE;
  }

  DeliveryState MsgBusAmqp::unsubscribe(const std::string& topic)
  {
    return DeliveryState::DELI_STATE_UNAVAILABLE;
  }

  DeliveryState MsgBusAmqp::publish(const std::string& topic, const std::string& message)
  {
    return DeliveryState::DELI_STATE_UNAVAILABLE;
  }

  DeliveryState MsgBusAmqp::sendRequest(const std::string& requestQueue, const std::string& message, MessageListener<AmqpMessage> messageListener)
  {
    return DeliveryState::DELI_STATE_UNAVAILABLE;
  }

  Opt<AmqpMessage> MsgBusAmqp::sendRequest(const std::string& requestQueue, const std::string& message, int timeOut)
  {
    Opt<AmqpMessage> val{};
    return val;
  }

  DeliveryState MsgBusAmqp::waitRequest(const std::string& requestQueue, MessageListener<AmqpMessage> messageListener)
  {
    return DeliveryState::DELI_STATE_UNAVAILABLE;
  }

  DeliveryState MsgBusAmqp::sendReply(const std::string& response, const AmqpMessage& message)
  {
    return DeliveryState::DELI_STATE_UNAVAILABLE;
  }
} // namespace fty::messagebus
