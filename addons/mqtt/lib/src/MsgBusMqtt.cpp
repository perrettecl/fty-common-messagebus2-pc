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

namespace
{
  template <typename ClientType>
  bool isServiceAvailable(std::shared_ptr<ClientType> client)
  {
    bool serviceAvailable = client && client->is_connected();
    if (!serviceAvailable)
    {
      log_error("Mqtt service is unvailable");
    }
    return serviceAvailable;
  }
}

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
    // Cleaning all async/sync mqtt clients
    if (isServiceAvailable(m_asynClient))
    {
      log_debug("Asynchronous client cleaning");
      sendServiceStatus(DISCONNECTED_MSG);
      m_asynClient->disable_callbacks();
      m_asynClient->stop_consuming();
      m_asynClient->disconnect()->wait();
    }
    if (isServiceAvailable(m_synClient))
    {
      log_debug("Synchronous client cleaning");
      m_synClient->stop_consuming();
      m_synClient->disconnect();
    }
  }

  ComState MessageBusMqtt::connect()
  {
    auto status = ComState::COM_STATE_NO_CONTACT;
    mqtt::create_options opts(MQTTVERSION_5);

    m_asynClient = std::make_shared<mqtt::async_client>(m_endpoint, utils::getClientId("async-" + m_clientName), opts);
    m_synClient = std::make_shared<mqtt::client>(m_endpoint, utils::getClientId("sync-" + m_clientName), opts);

    // Connection options
    auto connOpts = mqtt::connect_options_builder()
                      .clean_session(false)
                      .mqtt_version(MQTTVERSION_5)
                      .keep_alive_interval(std::chrono::seconds(KEEP_ALIVE))
                      .automatic_reconnect(true)
                      //.automatic_reconnect(std::chrono::seconds(1), std::chrono::seconds(5))
                      .clean_start(true)
                      .will(mqtt::message{DISCOVERY_TOPIC + m_clientName + DISCOVERY_TOPIC_SUBJECT, {DISAPPEARED_MSG}, QOS, true})
                      .finalize();
    try
    {
      // Start consuming _before_ connecting, because we could get a flood
      // of stored messages as soon as the connection completes since
      // we're using a persistent (non-clean) session with the broker.
      m_asynClient->start_consuming();
      m_asynClient->connect(connOpts)->wait();

      m_synClient->start_consuming();
      m_synClient->connect(connOpts);

      // Callback(s)
      m_asynClient->set_callback(m_cb);
      m_synClient->set_callback(m_cb);
      m_asynClient->set_connected_handler([this](const std::string& cause) {
        m_cb.onConnected(cause);
      });

      m_asynClient->set_update_connection_handler([this](const mqtt::connect_data& connData) {
        return m_cb.onConnectionUpdated(connData);
      });
      m_synClient->set_update_connection_handler([this](const mqtt::connect_data& connData) {
        return m_cb.onConnectionUpdated(connData);
      });
      status = ComState::COM_STATE_OK;
      log_info("%s => connect status: sync client: %s, async client: %s", m_clientName.c_str(), m_asynClient->is_connected() ? "true" : "false",  m_synClient->is_connected() ? "true" : "false");
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

  DeliveryState MessageBusMqtt::publish(const std::string& topic, const Message& message)
  {
    auto delivState = DeliveryState::DELI_STATE_UNAVAILABLE;
    if (isServiceAvailable(m_asynClient))
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
      delivState = m_asynClient->publish(pubMsg)->wait_for(TIMEOUT) ? DeliveryState::DELI_STATE_ACCEPTED : DeliveryState::DELI_STATE_REJECTED;
      log_debug("Message published (%s)", to_string(delivState).c_str());
    }
    return delivState;
  }

  DeliveryState MessageBusMqtt::subscribe(const std::string& topic, MessageListener messageListener)
  {
    auto delivState = DeliveryState::DELI_STATE_UNAVAILABLE;
    if (isServiceAvailable(m_asynClient))
    {
      log_debug("Subscribing on topic: %s", topic.c_str());
      m_cb.setSubscriptions(topic, messageListener);
      m_asynClient->set_message_callback([this](mqtt::const_message_ptr msg) {
        // Wrapper from mqtt msg to Message
        m_cb.onMessageArrived(msg);
      });
      delivState = m_asynClient->subscribe(topic, QOS)->wait_for(TIMEOUT) ? DeliveryState::DELI_STATE_ACCEPTED : DeliveryState::DELI_STATE_REJECTED;
      log_debug("Subscribed (%s)", to_string(delivState).c_str());
    }
    return delivState;
  }

  DeliveryState MessageBusMqtt::unsubscribe(const std::string& topic, MessageListener /*messageListener*/)
  {
    auto delivState = DeliveryState::DELI_STATE_UNAVAILABLE;
    if (isServiceAvailable(m_asynClient))
    {
      log_trace("%s - unsubscribed for topic '%s'", m_clientName.c_str(), topic.c_str());
      delivState = m_asynClient->unsubscribe(topic)->wait_for(TIMEOUT) ? DeliveryState::DELI_STATE_ACCEPTED : DeliveryState::DELI_STATE_REJECTED;
      m_cb.eraseSubscriptions(topic);
    }
    return delivState;
  }

  DeliveryState MessageBusMqtt::receive(const std::string& queue, MessageListener messageListener)
  {
    auto delivState = DeliveryState::DELI_STATE_UNAVAILABLE;
    if (isServiceAvailable(m_asynClient))
    {
      m_cb.setSubscriptions(queue, messageListener);
      m_asynClient->set_message_callback([this](mqtt::const_message_ptr msg) {
        const mqtt::properties& props = msg->get_properties();
        if (/*props.contains(mqtt::property::RESPONSE_TOPIC) ||*/ props.contains(mqtt::property::CORRELATION_DATA))
        {
          // Wrapper from mqtt msg to Message
          m_cb.onMessageArrived(msg, m_asynClient);
        }
        else
        {
          log_error("Missing mqtt properties for Req/Rep (i.e. CORRELATION_DATA or RESPONSE_TOPIC");
        }
      });
      delivState = m_asynClient->subscribe(queue, QOS)->wait_for(TIMEOUT) ? DeliveryState::DELI_STATE_ACCEPTED : DeliveryState::DELI_STATE_REJECTED;
      log_debug("Waiting to receive msg from: %s", queue.c_str(), to_string(delivState).c_str());
    }
    return delivState;
  }

  DeliveryState MessageBusMqtt::sendRequest(const std::string& requestQueue, const Message& message)
  {
    auto delivState = DeliveryState::DELI_STATE_UNAVAILABLE;
    if (isServiceAvailable(m_asynClient))
    {
      // Adding all meta data inside mqtt properties
      auto props = getMqttPropertiesFromMetaData(message.metaData());

      auto replyTo = mqtt::get<std::string>(props, mqtt::property::RESPONSE_TOPIC);
      log_debug("Sending request payload: '%s' to: %s and wait message on reply queue %s", message.userData().c_str(), requestQueue.c_str(), replyTo.c_str());

      auto reqMsg = mqtt::message_ptr_builder()
                      .topic(requestQueue)
                      .payload(message.userData())
                      .qos(QOS)
                      .properties(props)
                      .retained(RETAINED)
                      .finalize();

      delivState = m_asynClient->publish(reqMsg)->wait_for(TIMEOUT) ? DeliveryState::DELI_STATE_ACCEPTED : DeliveryState::DELI_STATE_REJECTED;
      log_debug("Request sent (%s)", to_string(delivState).c_str());
    }
    return delivState;
  }

  DeliveryState MessageBusMqtt::sendRequest(const std::string& requestQueue, const Message& message, MessageListener messageListener)
  {
    auto delivState = receive(requestQueue, messageListener);
    if (delivState == DELI_STATE_ACCEPTED)
    {
      delivState = sendRequest(requestQueue, message);
    }
    return delivState;
  }

  DeliveryState MessageBusMqtt::sendReply(const std::string& replyQueue, const Message& message)
  {
    auto delivState = DeliveryState::DELI_STATE_UNAVAILABLE;
    if (isServiceAvailable(m_asynClient))
    {
      // Adding all meta data inside mqtt properties
      auto props = getMqttPropertiesFromMetaData(message.metaData());

      log_debug("Reply queue: '%s'", replyQueue.c_str());
      log_debug("Sending reply payload: '%s' to: %s", message.userData().c_str(), (mqtt::get<std::string>(props, mqtt::property::RESPONSE_TOPIC)).c_str());
      auto replyMsg = mqtt::message_ptr_builder()
                        .topic(replyQueue)
                        .payload(message.userData())
                        .qos(QOS)
                        .properties(props)
                        .retained(RETAINED)
                        .finalize();

      delivState = m_asynClient->publish(replyMsg)->wait_for(TIMEOUT) ? DeliveryState::DELI_STATE_ACCEPTED : DeliveryState::DELI_STATE_REJECTED;
      log_debug("Reply sent (%s)", to_string(delivState).c_str());
    }
    return delivState;
  }

  Opt<Message> MessageBusMqtt::request(const std::string& requestQueue, const Message& message, int receiveTimeOut)
  {
    auto replyMsg = Opt<Message>{};
    if (isServiceAvailable(m_asynClient) && isServiceAvailable(m_synClient))
    {
      mqtt::const_message_ptr msg;
      auto replyQueue = getReplyQueue(message);

      m_synClient->subscribe(replyQueue, QOS);
      sendRequest(requestQueue, message);

      auto messageArrived = m_synClient->try_consume_message_for(&msg, std::chrono::seconds(receiveTimeOut));
      m_synClient->unsubscribe(replyQueue);
      if (messageArrived)
      {
        log_debug("Message arrived (%s)", msg->get_payload_str().c_str());
        replyMsg = Message{getMetaDataFromMqttProperties(msg->get_properties()), msg->get_payload_str()};
      }
    }
    return replyMsg;
  }

  void MessageBusMqtt::sendServiceStatus(const std::string& message)
  {
    if (isServiceAvailable(m_asynClient))
    {
      auto topic = DISCOVERY_TOPIC + m_clientName + DISCOVERY_TOPIC_SUBJECT;
      auto msg = mqtt::message_ptr_builder()
                   .topic(topic)
                   .payload(message)
                   .qos(mqtt::ReasonCode::GRANTED_QOS_2)
                   .retained(true)
                   .finalize();
      bool status = m_asynClient->publish(msg)->wait_for(TIMEOUT);
      log_info("Service status %s => %s [%d]", m_clientName.c_str(), message.c_str(), status);
    }
  }

} // namespace fty::messagebus::mqttv5
