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

/*
@header
    fty_common_messagebus_mqtt -
@discuss
@end
*/

#include "fty/messagebus/mqtt/fty_common_messagebus_mqtt.hpp"
#include "fty_common_messagebus_exception.h"
#include "fty_common_messagebus_helper.hpp"
#include <fty_log.h>

#include <mqtt/async_client.h>
#include <mqtt/properties.h>

namespace
{
  using MqttMessage = messagebus::mqttv5::MqttMessage;

  static auto getMqttPropertiesFromMetaData(const messagebus::MetaData& metaData) -> const mqtt::properties
  {
    auto props = mqtt::properties{};
    for (const auto& data : metaData)
    {
      if (data.first == messagebus::REPLY_TO)
      {
        std::string correlationId = metaData.find(messagebus::CORRELATION_ID)->second;
        props.add({mqtt::property::CORRELATION_DATA, correlationId});
        props.add({mqtt::property::RESPONSE_TOPIC, data.second});
      }
      else if (data.first != messagebus::CORRELATION_ID)
      {
        props.add({mqtt::property::USER_PROPERTY, data.first, data.second});
      }
    }
    return props;
  }

  static auto getCorrelationId(const  MqttMessage& message) -> const std::string
  {
    auto iterator = message.metaData().find(messagebus::CORRELATION_ID);
    if (iterator == message.metaData().end() || iterator->second == "")
    {
      throw messagebus::MessageBusException("Request must have a correlation id.");
    }
    return iterator->second;
  }

} // namespace

namespace messagebus::mqttv5
{
  /////////////////////////////////////////////////////////////////////////////



  using duration = int64_t;
  duration KEEP_ALIVE = 20;
  static auto constexpr QOS = mqtt::ReasonCode::GRANTED_QOS_2;
  static auto constexpr RETAINED = false; //true;
  auto constexpr TIMEOUT = std::chrono::seconds(10);

  MqttMessageBus::~MqttMessageBus()
  {
    // Cleaning all async clients
    if (isServiceAvailable())
    {
      log_debug("Cleaning: %s", m_clientName.c_str());
      m_client->disable_callbacks();
      m_client->stop_consuming();
      m_client->disconnect()->wait();
    }
  }

  void MqttMessageBus::connect()
  {
    mqtt::create_options opts(MQTTVERSION_5);

    m_client = std::make_shared<mqtt::async_client>(m_endpoint, messagebus::helper::getClientId("etn"), opts);

    // Connection options
    auto connOpts = mqtt::connect_options_builder()
                      .clean_session()
                      .mqtt_version(MQTTVERSION_5)
                      .keep_alive_interval(std::chrono::seconds(KEEP_ALIVE))
                      .automatic_reconnect(true)
                      //.automatic_reconnect(std::chrono::seconds(2), std::chrono::seconds(30))
                      .clean_start(true)
                      .will(mqtt::message{WILL_TOPIC + m_clientName, {m_clientName + WILL_MSG}, QOS, true})
                      .finalize();

    try
    {
      // Start consuming _before_ connecting, because we could get a flood
      // of stored messages as soon as the connection completes since
      // we're using a persistent (non-clean) session with the broker.
      m_client->start_consuming();
      mqtt::token_ptr conntok = m_client->connect(connOpts);
      conntok->wait();
      // Callback
      m_client->set_callback(cb);
      m_client->set_connected_handler([this](const std::string& cause) {
        cb.onConnected(cause);
      });
      m_client->set_update_connection_handler([this](const mqtt::connect_data& connData) {
        return cb.onConnectionUpdated(connData);
      });
      log_info("%s => connect status: %s", m_clientName.c_str(), m_client->is_connected() ? "true" : "false");
    }
    catch (const mqtt::exception& exc)
    {
      log_error("Error to connect with the Mqtt server, raison: %s", exc.get_message().c_str());
    }
  }

  auto MqttMessageBus::isServiceAvailable() -> bool
  {
    return (m_client && m_client->is_connected());
  }

  void MqttMessageBus::publish(const std::string& topic, const MqttMessage& message)
  {
    if (isServiceAvailable())
    {
      log_debug("Publishing on topic: %s", topic.c_str());
      // Adding all meta data inside mqtt properties
      auto props = getMqttPropertiesFromMetaData(message.metaData());
      // Build the message
      auto pubMsg = mqtt::message_ptr_builder()
                      .topic(topic)
                      .payload(message.serialize())
                      .qos(QOS)
                      .properties(props)
                      .retained(false)
                      .finalize();
      // Publish it
      m_client->publish(pubMsg);
    }
  }

  void MqttMessageBus::subscribe(const std::string& topic, MessageListener messageListener)
  {
    if (isServiceAvailable())
    {
      log_debug("Subscribing on topic: %s", topic.c_str());
      cb.setSubscriptions(topic, messageListener);
      m_client->set_message_callback([this](mqtt::const_message_ptr msg) {
        // Wrapper from mqtt msg to Message
        cb.onMessageArrived(msg);
      });
      m_client->subscribe(topic, QOS);
    }
  }

  void MqttMessageBus::unsubscribe(const std::string& topic, MessageListener /*messageListener*/)
  {
    if (isServiceAvailable())
    {
      log_trace("%s - unsubscribed for topic '%s'", m_clientName.c_str(), topic.c_str());
      m_client->unsubscribe(topic)->wait();
    }
  }

  void MqttMessageBus::receive(const std::string& queue, MessageListener messageListener)
  {
    if (isServiceAvailable())
    {
      cb.setSubscriptions(queue, messageListener);
      m_client->set_message_callback([this](mqtt::const_message_ptr msg) {
        const mqtt::properties& props = msg->get_properties();
        if (/*props.contains(mqtt::property::RESPONSE_TOPIC) ||*/ props.contains(mqtt::property::CORRELATION_DATA))
        {
          // Wrapper from mqtt msg to Message
          cb.onMessageArrived(msg);
        }
        else
        {
          log_error("Missing mqtt properties for Req/Rep (i.e. CORRELATION_DATA or RESPONSE_TOPIC");
        }
      });

      log_debug("Waiting to receive msg from: %s", queue.c_str());
      m_client->subscribe(queue, QOS);
    }
  }

  void MqttMessageBus::sendRequest(const std::string& requestQueue, const MqttMessage& message)
  {
    if (isServiceAvailable())
    {
      // Adding all meta data inside mqtt properties
      auto props = getMqttPropertiesFromMetaData(message.metaData());

      auto replyTo = mqtt::get<std::string>(props, mqtt::property::RESPONSE_TOPIC);
      log_debug("Send request to: %s and wait to reply queue %s", requestQueue.c_str(), replyTo.c_str());

      auto reqMsg = mqtt::message_ptr_builder()
                      .topic(requestQueue)
                      .payload(message.serialize())
                      .qos(QOS)
                      .properties(props)
                      .retained(RETAINED)
                      .finalize();

      m_client->publish(reqMsg); //->wait_for(TIMEOUT);
      log_debug("Request sent");
    }
  }

  void MqttMessageBus::sendRequest(const std::string& requestQueue, const MqttMessage& message, MessageListener messageListener)
  {
    //auto replyTo = getReplyQueue(message);

    receive(requestQueue, messageListener);
    sendRequest(requestQueue, message);
  }

  void MqttMessageBus::sendReply(const std::string& replyQueue, const MqttMessage& message)
  {
    if (isServiceAvailable())
    {
      // Adding all meta data inside mqtt properties
      auto props = getMqttPropertiesFromMetaData(message.metaData());

      log_debug("Send reply to: %s", (mqtt::get<std::string>(props, mqtt::property::RESPONSE_TOPIC)).c_str());
      auto replyMsg = mqtt::message_ptr_builder()
                        .topic(replyQueue)
                        .payload(message.serialize())
                        .qos(QOS)
                        .properties(props)
                        .retained(RETAINED)
                        .finalize();

      m_client->publish(replyMsg);
    }
  }

  MqttMessage MqttMessageBus::request(const std::string& /*requestQueue*/, const MqttMessage& /*message*/, int /*receiveTimeOut*/)
  {
    if (isServiceAvailable())
    {
      // mqtt::const_message_ptr msg;
      // auto replyQueue = getReplyQueue(message);

      // m_client->subscribe(replyQueue, QOS);
      // sendRequest(requestQueue, message);
      // auto messageArrived = m_client->try_consume_message_for(&msg, std::chrono::seconds(receiveTimeOut));
      // if (messageArrived)
      // {
      //   return MqttMessage{getMetaDataFromMqttProperties(msg->get_properties()), msg->get_payload_str()};
      // }
      // else
      // {
      //   throw MessageBusException("Request timed out of '" + std::to_string(receiveTimeOut) + "' seconds reached.");
      // }
      //m_client->unsubscribe(replyQueue);
    }
     return MqttMessage{};
  }

} // namespace messagebus::mqttv5
