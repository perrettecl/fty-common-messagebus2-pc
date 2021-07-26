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

#pragma once

#include <fty/messagebus/ContainerInterface.hpp>
#include <fty/messagebus/MsgBusFactory.hpp>
#include <fty/messagebus/mqtt/MsgBusMqtt.hpp>
#include <fty/messagebus/utils/MsgBusHelper.hpp>

#include <memory>
#include <string>

namespace fty::messagebus
{

  // Default mqtt end point
  static auto constexpr DEFAULT_MQTT_END_POINT{"tcp://localhost:1883"};
  static auto constexpr SECURE_MQTT_END_POINT{"tcp://localhost:8883"};

  // Topic
  static const std::string PREFIX_TOPIC = "/etn/t";

  // Queues
  static const std::string PREFIX_QUEUE = "/etn/q/";
  static const std::string PREFIX_REQUEST_QUEUE = PREFIX_QUEUE + "request";
  static const std::string PREFIX_REPLY_QUEUE = PREFIX_QUEUE + "reply";

  using MqttMessage = fty::messagebus::mqttv5::MqttMessage;

  class MsgBusMqtt : public ContainerInterface<MqttMessage>
  {
  public:
    MsgBusMqtt(const std::string& endpoint = DEFAULT_MQTT_END_POINT, const std::string& clientName = utils::getClientId("MsgBusMqtt"));
    std::string identify() const override;

    DeliveryState subscribe(const std::string& topic, MessageListener<MqttMessage> messageListener) override;
    DeliveryState unsubscribe(const std::string& topic) override;
    DeliveryState publish(const std::string& topic, const std::string& message) override;

    DeliveryState sendRequest(const std::string& requestQueue, const std::string& message, MessageListener<MqttMessage> messageListener) override;
    Opt<MqttMessage> sendRequest(const std::string& requestQueue, const std::string& message, int timeOut) override;
    DeliveryState waitRequest(const std::string& requestQueue, MessageListener<MqttMessage> messageListener) override;
    DeliveryState sendReply(const std::string& response, const MqttMessage& message) override;

  private:
    std::string m_clientName{};
    std::unique_ptr<mqttv5::MessageBusMqtt> m_msgBus;
  };
} // namespace fty::messagebus
