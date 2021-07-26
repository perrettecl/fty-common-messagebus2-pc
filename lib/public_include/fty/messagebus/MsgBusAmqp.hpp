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

#pragma once

#include <fty/messagebus/ContainerInterface.hpp>
#include <fty/messagebus/MsgBusMessage.hpp>

#include <memory>
#include <string>

namespace fty::messagebus
{
  using AmqpMessage = fty::messagebus::MsgBusMessage;

  class MsgBusAmqp : public ContainerInterface<AmqpMessage>
  {
  public:
    MsgBusAmqp() = default;
    std::string identify() const override;

    DeliveryState subscribe(const std::string& topic, MessageListener<AmqpMessage> messageListener) override ;
    DeliveryState unsubscribe(const std::string& topic) override ;
    DeliveryState publish(const std::string& topic, const std::string& message) override;

    DeliveryState sendRequest(const std::string& requestQueue, const std::string& message, MessageListener<AmqpMessage> messageListener) override;
    Opt<AmqpMessage> sendRequest(const std::string& requestQueue, const std::string& message, int timeOut) override;
    DeliveryState waitRequest(const std::string& requestQueue, MessageListener<AmqpMessage> messageListener) override;
    DeliveryState sendReply(const std::string& response, const AmqpMessage& message) override;

  };
} // namespace fty::messagebus
