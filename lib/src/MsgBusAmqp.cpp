/*  =========================================================================
    MsgBusAmqp.cpp - class description

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

#include <fty/messagebus/MsgBusAmqp.hpp>

namespace fty::messagebus
{

  using AmqpMessage = fty::messagebus::amqp::AmqpMessage;
  using UserData = fty::messagebus::amqp::UserData;

  static constexpr auto AMQP_IMPL = "Message bus above Amqp implementation";

  MsgBusAmqp::MsgBusAmqp(const ClientName& clientName, const Endpoint& endpoint)
    : MsgBusWrapper(clientName, endpoint, AMQP_IMPL)
  {
  }

  DeliveryState MsgBusAmqp::subscribe(const std::string& /*topic*/, MessageListener<AmqpMessage> /*messageListener*/)
  {
    return DeliveryState::DELI_STATE_UNAVAILABLE;
  }

  DeliveryState MsgBusAmqp::unsubscribe(const std::string& /*topic*/)
  {
    return DeliveryState::DELI_STATE_UNAVAILABLE;
  }

  DeliveryState MsgBusAmqp::publish(const std::string& /*topic*/, const UserData& /*msg*/)
  {
    return DeliveryState::DELI_STATE_UNAVAILABLE;
  }

  DeliveryState MsgBusAmqp::sendRequest(const std::string& /*requestQueue*/, const UserData& /*msg*/, MessageListener<AmqpMessage> /*messageListener*/)
  {
    return DeliveryState::DELI_STATE_UNAVAILABLE;
  }

  Opt<AmqpMessage> MsgBusAmqp::sendRequest(const std::string& /*requestQueue*/, const UserData& /*msg*/, int /*timeOut*/)
  {
    Opt<AmqpMessage> val{};
    return val;
  }

  DeliveryState MsgBusAmqp::registerRequestListener(const std::string& /*requestQueue*/, MessageListener<AmqpMessage> /*messageListener*/)
  {
    return DeliveryState::DELI_STATE_UNAVAILABLE;
  }

  DeliveryState MsgBusAmqp::sendRequestReply(const AmqpMessage& /*inputRequest*/, const UserData& /*response*/)
  {
    return DeliveryState::DELI_STATE_UNAVAILABLE;
  }

  AmqpMessage MsgBusAmqp::buildMessage(const std::string& /*queue*/, const UserData& msg)
  {
    AmqpMessage message;
    message.userData() = msg;
    return message;
  }
} // namespace fty::messagebus
