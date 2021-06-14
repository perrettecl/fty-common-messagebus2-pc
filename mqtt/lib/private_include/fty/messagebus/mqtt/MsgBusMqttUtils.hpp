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

#ifndef FTY_COMMON_MESSAGEBUS_MQTT_UTILS
#define FTY_COMMON_MESSAGEBUS_MQTT_UTILS

#include "fty/messagebus/mqtt/MsgBusMqttMessage.hpp"

#include <mqtt/async_client.h>
#include <mqtt/client.h>
#include <mqtt/message.h>

namespace fty::messagebus::mqttv5
{
  using Message = MqttMessage;

  static auto getMetaDataFromMqttProperties(const mqtt::properties& props) -> const MetaData
  {
    auto metaData = MetaData{};

    // User properties
    if (props.contains(mqtt::property::USER_PROPERTY))
    {
      std::string key, value;
      for (size_t i = 0; i < props.count(mqtt::property::USER_PROPERTY); i++)
      {
        std::tie(key, value) = mqtt::get<mqtt::string_pair>(props, mqtt::property::USER_PROPERTY, i);
        metaData.emplace(key, value);
      }
    }
    // Req/Rep pattern properties
    if (props.contains(mqtt::property::CORRELATION_DATA))
    {
      metaData.emplace(CORRELATION_ID, mqtt::get<std::string>(props, mqtt::property::CORRELATION_DATA));
    }

    if (props.contains(mqtt::property::RESPONSE_TOPIC))
    {
      metaData.emplace(REPLY_TO, mqtt::get<std::string>(props, mqtt::property::RESPONSE_TOPIC));
    }
    return metaData;
  }

  static auto getMqttPropertiesFromMetaData(const MetaData& metaData) -> const mqtt::properties
  {
    auto props = mqtt::properties{};
    for (const auto& data : metaData)
    {
      if (data.first == REPLY_TO)
      {
        std::string correlationId = metaData.find(CORRELATION_ID)->second;
        props.add({mqtt::property::CORRELATION_DATA, correlationId});
        props.add({mqtt::property::RESPONSE_TOPIC, data.second});
      }
      else if (data.first != CORRELATION_ID)
      {
        props.add({mqtt::property::USER_PROPERTY, data.first, data.second});
      }
    }
    return props;
  }

  static auto getCorrelationId(const Message& message) -> const std::string
  {
    auto iterator = message.metaData().find(CORRELATION_ID);
    if (iterator == message.metaData().end() || iterator->second == "")
    {
      throw MessageBusException("Request must have a correlation id.");
    }
    return iterator->second;
  }

  static auto getReplyQueue(const Message& message) -> const std::string
  {
    auto iterator = message.metaData().find(REPLY_TO);
    if (iterator == message.metaData().end() || iterator->second == "")
    {
      throw MessageBusException("Request must have a reply to.");
    }
    return iterator->second;
  }

} // namespace fty::messagebus::mqttv5

#endif // ifndef FTY_COMMON_MESSAGEBUS_MQTT
