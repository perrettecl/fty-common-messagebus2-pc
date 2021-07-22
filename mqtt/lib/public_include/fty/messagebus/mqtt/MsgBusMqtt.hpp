/*  =========================================================================
    fty_common_messagebus_mqtt - class description

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


#include "fty/messagebus/mqtt/MsgBusMqttCallBack.hpp"

namespace fty::messagebus::mqttv5
{
  using Message = fty::messagebus::mqttv5::MqttMessage;

  // Mqtt default delimiter
  static auto constexpr MQTT_DELIMITER{'/'};

  // Mqtt will topic
  static auto constexpr DISCOVERY_TOPIC{"/etn/t/service/"};
  static auto constexpr DISCOVERY_TOPIC_SUBJECT{"/status"};

  // Mqtt will message
  static auto constexpr CONNECTED_MSG{"CONNECTED"};
  static auto constexpr DISCONNECTED_MSG{"DISCONNECTED"};
  static auto constexpr DISAPPEARED_MSG{"DISAPPEARED"};

  class MessageBusMqtt final : public IMessageBus<Message>
  {
  public:
    MessageBusMqtt() = delete;

    MessageBusMqtt(const std::string& endpoint, const std::string& clientName)
      : m_endpoint(endpoint)
      , m_clientName(clientName){};

    ~MessageBusMqtt() override;

    [[nodiscard]] fty::messagebus::ComState connect() override;

    // Pub/Sub pattern
    DeliveryState publish(const std::string& topic, const Message& message) override;
    DeliveryState subscribe(const std::string& topic, MessageListener messageListener) override;
    DeliveryState unsubscribe(const std::string& topic, MessageListener messageListener = {}) override;

    // Req/Rep pattern
    DeliveryState sendRequest(const std::string& requestQueue, const Message& message) override;
    DeliveryState sendRequest(const std::string& requestQueue, const Message& message, MessageListener messageListener) override;
    DeliveryState sendReply(const std::string& replyQueue, const Message& message) override;
    DeliveryState receive(const std::string& queue, MessageListener messageListener) override;

    // Sync queue
    Opt<Message> request(const std::string& requestQueue, const Message& message, int receiveTimeOut) override;

  private:
    ClientPointer m_client;

    std::string m_endpoint{};
    std::string m_clientName{};

    // Call back
    CallBack cb;

    void sendServiceStatus(const std::string& message);
    bool isServiceAvailable();
  };
} // namespace fty::messagebus::mqttv5
