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

#include "fty_common_messagebus_message.h"
#include "mqtt/async_client.h"
#include "mqtt/properties.h"

#include <fty_log.h>
#include <iostream>
#include <vector>

namespace
{

  using namespace messagebus;

  /**
   *
   *
   */
  static Message _fromMqttMsg(mqtt::const_message_ptr msg)
  {
    Message message{};

    // Meta data
    message.metaData().emplace(Message::SUBJECT, msg->get_topic());
    message.userData().emplace_back(msg->get_payload_str());

    // zframe_t *item;

    // if( zmsg_size(msg) ) {
    //     item = zmsg_pop(msg);
    //     std::string key((const char *)zframe_data(item), zframe_size(item));
    //     zframe_destroy(&item);
    //     if( key == "__METADATA_START" ) {
    //         while ((item = zmsg_pop(msg))) {
    //             key = std::string((const char *)zframe_data(item), zframe_size(item));
    //             zframe_destroy(&item);
    //             if (key == "__METADATA_END") {
    //                 break;
    //             }
    //             zframe_t *zvalue = zmsg_pop(msg);
    //             std::string value((const char *)zframe_data(zvalue), zframe_size(zvalue));
    //             zframe_destroy(&item);
    //             message.metaData().emplace(key, value);
    //         }
    //     }
    //     else {
    //         message.userData().emplace_back(key);
    //     }
    //     while ((item = zmsg_pop(msg))) {
    //         message.userData().emplace_back((const char *)zframe_data(item), zframe_size(item));
    //         zframe_destroy(&item);
    //     }
    // }
    return message;
  }

} // namespace

namespace messagebus
{
  /////////////////////////////////////////////////////////////////////////////

  using duration = int64_t;
  //auto constexpr SERVER_ADDRESS{"tcp://localhost:1883"};
  duration KEEP_ALIVE = 20;
  auto constexpr QOS = mqtt::ReasonCode::GRANTED_QOS_1;
  auto constexpr TIMEOUT = std::chrono::seconds(10);

  MqttMessageBus::~MqttMessageBus()
  {
    // Cleaning all async clients
    if (m_client->is_connected())
    {
      m_client->disable_callbacks();
      m_client->stop_consuming();
      m_client->disconnect()->wait();
    }

    if (m_clientReqRep->is_connected())
    {
      m_clientReqRep->disable_callbacks();
      m_clientReqRep->stop_consuming();
      m_clientReqRep->disconnect()->wait();
    }
  }

  void MqttMessageBus::connect()
  {
    mqtt::create_options opts(MQTTVERSION_5);

    m_client = std::make_shared<mqtt::async_client>(m_endpoint, messagebus::getClientId("etn"), opts);
    m_clientReqRep = std::make_shared<mqtt::async_client>(m_endpoint, messagebus::getClientId("etn"), opts);

    auto connOpts = mqtt::connect_options_builder()
                      .clean_session()
                      .mqtt_version(MQTTVERSION_5)
                      .keep_alive_interval(std::chrono::seconds(KEEP_ALIVE))
                      .automatic_reconnect(std::chrono::seconds(2), std::chrono::seconds(30))
                      .clean_start(true)
                      .finalize();

    m_client->set_message_callback([this](mqtt::const_message_ptr msg) {
      MqttMessageBus::onMessageArrived(msg);
    });

    m_clientReqRep->set_message_callback([this](mqtt::const_message_ptr msg) {
      MqttMessageBus::onMessageArrived(msg);
    });

    m_client->set_connection_lost_handler([this](const std::string& cause) {
      MqttMessageBus::onConnectionLost(cause);
    });

    m_clientReqRep->set_connection_lost_handler([this](const std::string& cause) {
      MqttMessageBus::onConnectionLost(cause);
    });

    try
    {
      // Start consuming _before_ connecting, because we could get a flood
      // of stored messages as soon as the connection completes since
      // we're using a persistent (non-clean) session with the broker.
      m_client->start_consuming();
      mqtt::token_ptr conntok = m_client->connect(connOpts);
      conntok->wait();
      log_info("Connect status: %b", m_client->is_connected());

      m_clientReqRep->start_consuming();
      conntok = m_clientReqRep->connect(connOpts);
      conntok->wait();
      log_info("Connect status: %b", m_clientReqRep->is_connected());
    }
    catch (const mqtt::exception& exc)
    {
      log_error("Error to connect with the Mqtt server, raison: %s", exc.get_error_str());
    }
  }

  // Callback called the connection lost.
  void MqttMessageBus::onConnectionLost(const std::string& cause)
  {
    log_error("Connection lost");
    if (!cause.empty())
    {
      log_error("raison: %s", cause.c_str());
    }
  }

  // Callback called when a message arrives.
  void MqttMessageBus::onMessageArrived(mqtt::const_message_ptr msg)
  {
    log_trace("%s - received stream message from '%s' subject", m_clientName.c_str(), msg->get_topic());
    messagebus::Message message = _fromMqttMsg(msg);

    auto iterator = m_subscriptions.find(msg->get_topic());
    if (iterator != m_subscriptions.end())
    {
      try
      {
        (iterator->second)(message);
      }
      catch (const std::exception& e)
      {
        log_error("Error in listener of topic '%s': '%s'", iterator->first.c_str(), e.what());
      }
      catch (...)
      {
        log_error("Error in listener of topic '%s': 'unknown error'", iterator->first.c_str());
      }
    }
  }

  void MqttMessageBus::publish(const std::string& topic, const Message& /*message*/)
  {
    log_debug("Publishing on topic: %s...", topic);
    // TODO convert to mqtt message
    mqtt::message_ptr pubmsg = mqtt::make_message(topic, "message");
    pubmsg->set_qos(QOS);
    m_client->publish(pubmsg)->wait_for(TIMEOUT);
  }

  void MqttMessageBus::subscribe(const std::string& topic, MessageListener messageListener)
  {
    log_debug("Subscribing on topic: %s...", topic);
    m_subscriptions.emplace(topic, messageListener);
    m_client->subscribe(topic, QOS);
  }

  void MqttMessageBus::unsubscribe(const std::string& topic, MessageListener /*messageListener*/)
  {
    auto iterator = m_subscriptions.find(topic);
    if (iterator == m_subscriptions.end())
    {
      throw MessageBusException("Trying to unsubscribe on non-subscribed topic.");
    }

    m_subscriptions.erase(iterator);
    log_trace("%s - unsubscribed to topic '%s'", m_clientName.c_str(), topic.c_str());
    m_client->unsubscribe(topic)->wait();
  }

  void MqttMessageBus::sendRequest(const std::string& /*requestQueue*/, const Message& /*message*/)
  {
    if (m_clientReqRep)
    {
      std::string reqTopic = "requestQueue/test/";
      std::string repTopic = "repliesQueue/clientId";

      mqtt::token_ptr tokPtr = m_client->subscribe(repTopic, QOS);
      tokPtr->wait();

      if (int(tokPtr->get_reason_code()) != QOS)
      {
        log_error("Error: Server doesn't support reply QoS: %s", tokPtr->get_reason_code());
      }
      else
      {
        mqtt::properties props{
          {mqtt::property::RESPONSE_TOPIC, repTopic},
          {mqtt::property::CORRELATION_DATA, "1"}};

        std::string reqArgs{"requestTest"};

        auto pubmsg = mqtt::message_ptr_builder()
                        .topic(reqTopic)
                        .payload(reqArgs)
                        .qos(QOS)
                        .properties(props)
                        .finalize();

        m_clientReqRep->publish(pubmsg)->wait_for(TIMEOUT);
      }
    }
  }

  void MqttMessageBus::sendRequest(const std::string& requestQueue, const Message& message, MessageListener messageListener)
  {
    auto iterator = message.metaData().find(Message::REPLY_TO);
    if (iterator == message.metaData().end() || iterator->second == "")
    {
      throw MessageBusException("Request must have a reply queue.");
    }
    std::string queue(iterator->second);
    receive(queue, messageListener);
    sendRequest(requestQueue, message);
  }

  void MqttMessageBus::sendReply(const std::string& /*replyQueue*/, const Message& /*message*/)
  {
    if(m_clientReqRep)
    {
      try
      {
        const std::vector<std::string> topics{"requests/math", "requests/math/#"};
        const std::vector<int> qos{1, 1};

        //m_clientReqRep.subscribe(topics, qos);

        //auto msg = cli.try_consume_message_for(std::chrono::seconds(5));
        bool msg = true;
        if (msg)
        {
          // const mqtt::properties& props = msg->get_properties();
          // if (props.contains(mqtt::property::RESPONSE_TOPIC) && props.contains(mqtt::property::CORRELATION_DATA))
          // {
          //   mqtt::binary corr_id = mqtt::get<std::string>(props, mqtt::property::CORRELATION_DATA);
          //   std::string reply_to = mqtt::get<std::string>(props, mqtt::property::RESPONSE_TOPIC);
          //   auto reply_msg = mqtt::message::create(reply_to, "response", 1, false);
          //   cli.publish(reply_msg);
          // }

          // std::cout << "  Result: " << msg->to_string() << std::endl;
        }
        else
        {
          std::cerr << "Didn't receive a reply from the service." << std::endl;
        }
      }
      catch (const mqtt::exception& exc)
      {
        log_error("Error to send a reply, raison: %s", exc.get_error_str());
      }
    }
  }

  void MqttMessageBus::receive(const std::string& queue, MessageListener messageListener)
  {
    auto iterator = m_subscriptions.find(queue);
    if (iterator != m_subscriptions.end())
    {
      throw MessageBusException("Already have queue map to listener");
    }
    m_subscriptions.emplace(queue, messageListener);
    m_clientReqRep->subscribe(queue, QOS);
  }

} // namespace messagebus