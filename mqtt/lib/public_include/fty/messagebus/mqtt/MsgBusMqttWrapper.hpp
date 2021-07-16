/*  =========================================================================
    MsgBusMqttWrapper - class description

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

#pragma once

#include <fty/messagebus/IMessageBus.hpp>
#include <fty/messagebus/MsgBusFactory.hpp>
#include <fty/messagebus/mqtt/MsgBusMqttDef.hpp>
#include <fty/messagebus/mqtt/MsgBusMqttMessage.hpp>
#include <fty/messagebus/utils/MsgBusHelper.hpp>

#include <thread>

namespace fty::messagebus::mqttv5
{

  using Message = fty::messagebus::mqttv5::MqttMessage;
  using MessageListener = fty::messagebus::MessageListener<Message>;
  using MsgBusPointer = std::unique_ptr<fty::messagebus::IMessageBus<Message>>;

  class MsgBusMqttWrapper
  {
  public:
    MsgBusMqttWrapper(const std::string& endpoint, const std::string& clientName)
      : m_clientName(clientName)
      , m_msgBus{fty::messagebus::MessageBusFactory::createMqttMsgBus(endpoint, clientName)}
    {
      m_msgBus->connect();
    };

    ~MsgBusMqttWrapper() = default;

  protected:
    std::string m_clientName{};
    MsgBusPointer m_msgBus;
  };

} // namespace fty::messagebus::mqttv5
