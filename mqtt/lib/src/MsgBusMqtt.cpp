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

#include "fty/messagebus/mqtt/MsgBusMqtt.hpp"
#include "fty/messagebus/MsgBusException.hpp"
#include "fty/messagebus/mqtt/MsgBusMqttUtils.hpp"
#include "fty/messagebus/utils/MsgBusHelper.hpp"

#include <fty_log.h>

#include <mqtt/async_client.h>
#include <mqtt/properties.h>

using namespace fty::messagebus;

namespace fty::messagebus::mqttv5
{
  using duration = int64_t;
  using Message = fty::messagebus::mqttv5::MqttMessage;

  duration KEEP_ALIVE = 20;
  static auto constexpr QOS = mqtt::ReasonCode::GRANTED_QOS_2;
  static auto constexpr RETAINED = false; //true;s
  auto constexpr TIMEOUT = std::chrono::seconds(10);

  MessageBusMqtt::~MessageBusMqtt()
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

  void MessageBusMqtt::connect()
  {
    mqtt::create_options opts(MQTTVERSION_5);

    m_client = std::make_shared<mqtt::async_client>(m_endpoint, utils::getClientId("etn"), opts);

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
      throw MessageBusException("Error to connect with the Mqtt server, reason: " + exc.get_message());
    }
    catch (const std::exception& exc)
    {
      throw MessageBusException("Unexpected error: " + std::string{exc.what()});
    }
  }

  auto MessageBusMqtt::isServiceAvailable() -> bool
  {
    return (m_client && m_client->is_connected());
  }

  void MessageBusMqtt::publish(const std::string& topic, const Message& message)
  {
    if (isServiceAvailable())
    {
      log_debug("Publishing on topic: %s", topic.c_str());
      // Adding all meta data inside mqtt properties
      auto props = getMqttPropertiesFromMetaData(message.metaData());
      // Build the message
      auto pubMsg = mqtt::message_ptr_builder()
                      .topic(topic)
                      .payload(message.userData())
                      .qos(QOS)
                      .properties(props)
                      .retained(false)
                      .finalize();
      // Publish it
      m_client->publish(pubMsg);
    }
  }

  void MessageBusMqtt::subscribe(const std::string& topic, MessageListener messageListener)
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

  void MessageBusMqtt::unsubscribe(const std::string& topic, MessageListener /*messageListener*/)
  {
    if (isServiceAvailable())
    {
      log_trace("%s - unsubscribed for topic '%s'", m_clientName.c_str(), topic.c_str());
      m_client->unsubscribe(topic)->wait();
    }
  }

  void MessageBusMqtt::receive(const std::string& queue, MessageListener messageListener)
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

  void MessageBusMqtt::sendRequest(const std::string& requestQueue, const Message& message)
  {
    if (isServiceAvailable())
    {
      // Adding all meta data inside mqtt properties
      auto props = getMqttPropertiesFromMetaData(message.metaData());

      auto replyTo = mqtt::get<std::string>(props, mqtt::property::RESPONSE_TOPIC);
      log_debug("Send request to: %s and wait to reply queue %s", requestQueue.c_str(), replyTo.c_str());

      auto reqMsg = mqtt::message_ptr_builder()
                      .topic(requestQueue)
                      .payload(message.userData())
                      .qos(QOS)
                      .properties(props)
                      .retained(RETAINED)
                      .finalize();

      m_client->publish(reqMsg); //->wait_for(TIMEOUT);
      log_debug("Request sent");
    }
  }

  void MessageBusMqtt::sendRequest(const std::string& requestQueue, const Message& message, MessageListener messageListener)
  {
    //auto replyTo = getReplyQueue(message);

    receive(requestQueue, messageListener);
    sendRequest(requestQueue, message);
  }

  void MessageBusMqtt::sendReply(const std::string& replyQueue, const Message& message)
  {
    if (isServiceAvailable())
    {
      // Adding all meta data inside mqtt properties
      auto props = getMqttPropertiesFromMetaData(message.metaData());

      log_debug("Send reply to: %s", (mqtt::get<std::string>(props, mqtt::property::RESPONSE_TOPIC)).c_str());
      auto replyMsg = mqtt::message_ptr_builder()
                        .topic(replyQueue)
                        .payload(message.userData())
                        .qos(QOS)
                        .properties(props)
                        .retained(RETAINED)
                        .finalize();

      m_client->publish(replyMsg);
    }
  }

  Message MessageBusMqtt::request(const std::string& requestQueue, const Message& message, int receiveTimeOut)
  {
    if (isServiceAvailable())
    {
      mqtt::const_message_ptr msg;
      auto replyQueue = getReplyQueue(message);

      m_client->subscribe(replyQueue, QOS);
      sendRequest(requestQueue, message);
      auto messageArrived = m_client->try_consume_message_for(&msg, std::chrono::seconds(receiveTimeOut));
      if (messageArrived)
      {
        return Message{getMetaDataFromMqttProperties(msg->get_properties()), msg->get_payload_str()};
      }
      else
      {
        throw MessageBusException("Request timed out of '" + std::to_string(receiveTimeOut) + "' seconds reached.");
      }
      m_client->unsubscribe(replyQueue);
    }
    return Message{};
  }

} // namespace fty::messagebus::mqttv5
