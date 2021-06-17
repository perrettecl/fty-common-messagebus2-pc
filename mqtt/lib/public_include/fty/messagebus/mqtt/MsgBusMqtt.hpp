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

#ifndef FTY_COMMON_MESSAGEBUS_MQTT_HPP
#define FTY_COMMON_MESSAGEBUS_MQTT_HPP

#include "fty/messagebus/IMessageBus.hpp"
#include "fty/messagebus/mqtt/MsgBusMqttCallBack.hpp"

#include <mqtt/async_client.h>
#include <mqtt/client.h>
#include <mqtt/message.h>

namespace fty::messagebus::mqttv5
{
  // Mqtt default delimiter
  static auto constexpr MQTT_DELIMITER{'/'};

  // Mqtt will topic
  static auto constexpr DISCOVERY_TOPIC{"/etn/t/service/"};
  static auto constexpr DISCOVERY_TOPIC_SUBJECT{"/status"};

  // Mqtt will message
  static auto constexpr CONNECTED_MSG{"CONNECTED"};
  static auto constexpr DISCONNECTED_MSG{"DISCONNECTED"};
  static auto constexpr DISAPPEARED_MSG{"DISAPPEARED"};

  using ClientPointer = std::shared_ptr<mqtt::async_client>;

  class MessageBusMqtt final : public IMessageBus<MqttMessage>
  {
  public:
    MessageBusMqtt() = default;

    MessageBusMqtt(const std::string& endpoint, const std::string& clientName)
      : m_endpoint(endpoint)
      , m_clientName(clientName){};

    ~MessageBusMqtt() override;

    void connect() override;

    // Pub/Sub pattern
    void publish(const std::string& topic, const MqttMessage& message) override;
    void subscribe(const std::string& topic, MessageListener messageListener) override;
    void unsubscribe(const std::string& topic, MessageListener messageListener = {}) override;

    // Req/Rep pattern
    void sendRequest(const std::string& requestQueue, const MqttMessage& message) override;
    void sendRequest(const std::string& requestQueue, const MqttMessage& message, MessageListener messageListener) override;
    void sendReply(const std::string& replyQueue, const MqttMessage& message) override;
    void receive(const std::string& queue, MessageListener messageListener) override;

    // Sync queue
    MqttMessage request(const std::string& requestQueue, const MqttMessage& message, int receiveTimeOut) override;

    auto isServiceAvailable() -> bool;

  private:
    ClientPointer m_client;

    std::string m_endpoint{};
    std::string m_clientName{};
    // Call back
    CallBack cb;

    void sendServiceStatus(const std::string& message);
  };
} // namespace fty::messagebus::mqttv5

#endif // ifndef FTY_COMMON_MESSAGEBUS_MQTT_HPP
