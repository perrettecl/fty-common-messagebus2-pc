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
#include <fty/messagebus/MsgBusException.hpp>
#include <fty/messagebus/MsgBusFactory.hpp>

#include <functional>
#include <memory>
#include <optional>
#include <string>

namespace fty::messagebus
{
  template <typename MessageBusType,
            typename MessageType,
            typename UserDataType>
  class IMessageBusWrapper
  {
  public:
    IMessageBusWrapper() = default;
    IMessageBusWrapper(const std::string& endpoint, const std::string& clientName)
      : m_endpoint(clientName)
      , m_clientName(endpoint)
      , m_msgBus{fty::messagebus::MessageBusFactory<MessageBusType>::createMsgBus(endpoint, clientName)}
    {
      auto state = m_msgBus->connect();
      if (state != fty::messagebus::COM_STATE_OK)
      {
        throw MessageBusException("Mqtt server connection error");
      }
    };
    virtual ~IMessageBusWrapper() = default;

    // Witch implementation
    virtual std::string identify() const = 0;

    // Publish/Subcribe  pattern
    virtual DeliveryState subscribe(const std::string& topic, MessageListener<MessageType> messageListener) = 0;
    virtual DeliveryState unsubscribe(const std::string& topic) = 0;
    virtual DeliveryState publish(const std::string& topic, const UserDataType& msg) = 0;

    // Request/Reply  pattern
    virtual DeliveryState sendRequest(const std::string& requestQueue, const UserDataType& msg, MessageListener<MessageType> messageListener) = 0;
    virtual Opt<MessageType> sendRequest(const std::string& requestQueue, const UserDataType& msg, int timeOut) = 0;
    virtual DeliveryState waitRequest(const std::string& requestQueue, MessageListener<MessageType> messageListener) = 0;
    virtual DeliveryState sendReply(const UserDataType& response, const MessageType& message) = 0;

  protected:
    std::string m_endpoint{};
    std::string m_clientName{};
    std::unique_ptr<MessageBusType> m_msgBus;

    virtual MessageType buildMessage(const std::string& queue, const UserDataType& msg) = 0;
  };

} // namespace fty::messagebus
