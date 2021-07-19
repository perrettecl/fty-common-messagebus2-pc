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
#include <mqtt/client.h>
#include <mqtt/message.h>
#include <mqtt/properties.h>

namespace fty::messagebus::mqttv5
{
  using namespace fty::messagebus;

  using duration = int64_t;
  using Message = fty::messagebus::mqttv5::MqttMessage;

  duration KEEP_ALIVE = 20;
  static auto constexpr QOS = mqtt::ReasonCode::GRANTED_QOS_2;
  static auto constexpr RETAINED = false; //true;
  auto constexpr TIMEOUT = std::chrono::seconds(5);

  MessageBusMqtt::~MessageBusMqtt()
  {
    // Cleaning all async clients
    if (isServiceAvailable())
    {
      log_debug("Cleaning: %s", m_clientName.c_str());
      sendServiceStatus(DISCONNECTED_MSG);
      m_client->disable_callbacks();
      m_client->stop_consuming();
      m_client->disconnect()->wait();
    }
    else
    {
      log_error("Cleaning: %s", m_clientName.c_str());
    }
  }

  ComState MessageBusMqtt::connect()
  {
    auto status = ComState::COM_STATE_NO_CONTACT;
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
                      .will(mqtt::message{DISCOVERY_TOPIC + m_clientName + DISCOVERY_TOPIC_SUBJECT, {DISAPPEARED_MSG}, QOS, true})
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
      status = ComState::COM_STATE_OK;
      log_info("%s => connect status: %s", m_clientName.c_str(), m_client->is_connected() ? "true" : "false");
      sendServiceStatus(CONNECTED_MSG);
    }
    catch (const mqtt::exception& e)
    {
      status = ComState::COM_STATE_NONE;
      log_error("Error to connect with the Mqtt server, reason: %s", e.get_message().c_str());
    }
    catch (const std::exception& e)
    {
      status = ComState::COM_STATE_UNKNOWN;
      log_error("Unexpected error: %s", e.what());
    }
    return status;
  }

  auto MessageBusMqtt::isServiceAvailable() -> bool
  {
    bool serviceAvailable = m_client && m_client->is_connected();
    if (!serviceAvailable)
    {
      log_error("Mqtt service is unvailable");
    }
    return serviceAvailable;
  }

  void MessageBusMqtt::publish(const std::string& topic, const Message& message)
  {
    bool status = false;
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
      status = m_client->publish(pubMsg)->wait_for(TIMEOUT);
    }
    //return status;
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
      m_client->subscribe(topic, QOS)->wait_for(TIMEOUT);
    }
  }

  void MessageBusMqtt::unsubscribe(const std::string& topic, MessageListener /*messageListener*/)
  {
    if (isServiceAvailable())
    {
      log_trace("%s - unsubscribed for topic '%s'", m_clientName.c_str(), topic.c_str());
      m_client->unsubscribe(topic)->wait_for(TIMEOUT);
      cb.eraseSubscriptions(topic);
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
          cb.onMessageArrived(msg, m_client);
        }
        else
        {
          log_error("Missing mqtt properties for Req/Rep (i.e. CORRELATION_DATA or RESPONSE_TOPIC");
        }
      });

      log_debug("Waiting to receive msg from: %s", queue.c_str());
      m_client->subscribe(queue, QOS)->wait_for(TIMEOUT);
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

      m_client->publish(reqMsg)->wait_for(TIMEOUT);
      log_debug("Request sent");
    }
  }

  void MessageBusMqtt::sendRequest(const std::string& requestQueue, const Message& message, MessageListener messageListener)
  {
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

      m_client->publish(replyMsg)->wait_for(TIMEOUT);
    }
  }

  Opt<Message> MessageBusMqtt::request(const std::string& requestQueue, const Message& message, int receiveTimeOut)
  {
    auto replyMsg = Opt<Message>{};
    if (isServiceAvailable())
    {
      mqtt::const_message_ptr msg;
      auto replyQueue = getReplyQueue(message);

      m_client->subscribe(replyQueue, QOS)->wait_for(TIMEOUT);
      sendRequest(requestQueue, message);
      auto messageArrived = m_client->try_consume_message_for(&msg, std::chrono::seconds(receiveTimeOut));
      m_client->unsubscribe(replyQueue)->wait_for(TIMEOUT);
      if (messageArrived)
      {
        replyMsg = Message{getMetaDataFromMqttProperties(msg->get_properties()), msg->get_payload_str()};
      }
    }
    return replyMsg;
  }

  void MessageBusMqtt::sendServiceStatus(const std::string& message)
  {
    auto topic = DISCOVERY_TOPIC + m_clientName + DISCOVERY_TOPIC_SUBJECT;
    auto msg = mqtt::message_ptr_builder()
                 .topic(topic)
                 .payload(message)
                 .qos(mqtt::ReasonCode::GRANTED_QOS_2)
                 .retained(true)
                 .finalize();
    mqtt::delivery_token_ptr pubtok = m_client->publish(msg);
    bool status = pubtok->wait_for(TIMEOUT);

    log_info("DISCOVERY %s => %s [%d]", m_clientName.c_str(), message.c_str(), status);
  }

} // namespace fty::messagebus::mqttv5
