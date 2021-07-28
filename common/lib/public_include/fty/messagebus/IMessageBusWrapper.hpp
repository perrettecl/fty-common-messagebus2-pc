/*  =========================================================================
    PublishSubscribe - class description

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

#include "fty/messagebus/IMessageBus.hpp"
#include "fty/messagebus/MsgBusStatus.hpp"

#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace fty::messagebus
{
  template <typename MessageType,
            typename T>
  class IMessageBusWrapper
  {
  public:
    IMessageBusWrapper() = default;
    virtual ~IMessageBusWrapper() = default;
    virtual std::string identify() const = 0;

    virtual DeliveryState subscribe(const std::string& topic, MessageListener<MessageType> messageListener) = 0;
    virtual DeliveryState unsubscribe(const std::string& topic) = 0;
    virtual DeliveryState publish(const std::string& topic, const T& msg) = 0;

    virtual DeliveryState sendRequest(const std::string& requestQueue, const T& msg, MessageListener<MessageType> messageListener) = 0;
    virtual Opt<MessageType> sendRequest(const std::string& requestQueue, const T& msg, int timeOut) = 0;
    virtual DeliveryState waitRequest(const std::string& requestQueue, MessageListener<MessageType> messageListener) = 0;
    virtual DeliveryState sendReply(const T& response, const MessageType& message) = 0;

  protected:
    //std::string m_clientName{};
    //std::unique_ptr<mqttv5::MessageBusMqtt> m_msgBus;

    MessageType buildMessage(const std::string& queue, const T& msg);
  };

} // namespace fty::messagebus
