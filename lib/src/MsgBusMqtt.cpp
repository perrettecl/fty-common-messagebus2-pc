/*  =========================================================================
    MsgBusMqtt.hpp - class description

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

#include "fty/messagebus/MsgBusMqtt.hpp"
#include <fty/messagebus/MsgBusException.hpp>

namespace fty::messagebus
{
  static constexpr auto MQTT_IMPL = "MQTT implementation";

  MsgBusMqtt::MsgBusMqtt(const std::string& endpoint, const std::string& clientName)
    : m_clientName(clientName)
    , m_msgBus{fty::messagebus::MessageBusFactory<mqttv5::MessageBusMqtt>::createMsgBus(endpoint, clientName)}
  {
    auto state = m_msgBus->connect();
    if (state != fty::messagebus::COM_STATE_OK)
    {
      throw MessageBusException("Mqtt server connection error");
    }
  }

  std::string MsgBusMqtt::identify() const
  {
    return MQTT_IMPL;
  }

  DeliveryState MsgBusMqtt::subscribe(const std::string& topic, MessageListener<MqttMessage> messageListener)
  {
    return m_msgBus->subscribe(PREFIX_TOPIC + topic, messageListener);
    //return DeliveryState::DELI_STATE_UNAVAILABLE;
  }

  DeliveryState MsgBusMqtt::unsubscribe(const std::string& topic)
  {
    return m_msgBus->unsubscribe(PREFIX_TOPIC + topic, nullptr);
  }

  DeliveryState MsgBusMqtt::publish(const std::string& topic, const std::string& msg)
  {
    MqttMessage message;
    message.userData() = msg;
    message.metaData().clear();
    message.metaData().emplace(SUBJECT, PUBLISH_USER_PROPERTY);
    message.metaData().emplace(FROM, m_clientName);

    return m_msgBus->publish(PREFIX_TOPIC + topic, message);
  }

  DeliveryState MsgBusMqtt::sendRequest(const std::string& requestQueue, const std::string& message, MessageListener<MqttMessage> messageListener)
  {
    return DeliveryState::DELI_STATE_UNAVAILABLE;
  }

  Opt<MqttMessage> MsgBusMqtt::sendRequest(const std::string& requestQueue, const std::string& message, int timeOut)
  {
    Opt<MqttMessage> val{};
    return val;
  }

  DeliveryState MsgBusMqtt::waitRequest(const std::string& requestQueue, MessageListener<MqttMessage> messageListener)
  {
    return DeliveryState::DELI_STATE_UNAVAILABLE;
  }

  DeliveryState MsgBusMqtt::sendReply(const std::string& response, const MqttMessage& message)
  {
    return DeliveryState::DELI_STATE_UNAVAILABLE;
  }
} // namespace fty::messagebus
