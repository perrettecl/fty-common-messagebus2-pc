/*  =========================================================================
    fty_common_messagebus_malamute - class description
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

/*
@header
    fty_common_messagebus_malamute -
@discuss
@end
*/

#include "fty/messagebus/mlm/MsgBusMalamute.hpp"

#include <thread>

namespace fty::messagebus::mlm
{
  using namespace fty::messagebus;

  static MlmMessage _fromZmsg(zmsg_t* msg)
  {
    MlmMessage message;
    zframe_t* item;

    if (zmsg_size(msg))
    {
      item = zmsg_pop(msg);
      std::string key(reinterpret_cast<const char*>(zframe_data(item)), zframe_size(item));
      zframe_destroy(&item);
      if (key == "__METADATA_START")
      {
        while ((item = zmsg_pop(msg)))
        {
          key = std::string(reinterpret_cast<const char*>(zframe_data(item)), zframe_size(item));
          zframe_destroy(&item);
          if (key == "__METADATA_END")
          {
            break;
          }
          zframe_t* zvalue = zmsg_pop(msg);
          std::string value(reinterpret_cast<const char*>(zframe_data(zvalue)), zframe_size(zvalue));
          zframe_destroy(&item);
          message.metaData().emplace(key, value);
        }
      }
      else
      {
        message.userData().emplace_back(key);
      }
      while ((item = zmsg_pop(msg)))
      {
        message.userData().emplace_back(reinterpret_cast<const char*>(zframe_data(item)), zframe_size(item));
        zframe_destroy(&item);
      }
    }
    return message;
  }

  static zmsg_t* _toZmsg(const MlmMessage& message)
  {
    zmsg_t* msg = zmsg_new();

    zmsg_addstr(msg, "__METADATA_START");
    for (const auto& pair : message.metaData())
    {
      zmsg_addmem(msg, pair.first.c_str(), pair.first.size());
      zmsg_addmem(msg, pair.second.c_str(), pair.second.size());
    }
    zmsg_addstr(msg, "__METADATA_END");
    for (const auto& item : message.userData())
    {
      zmsg_addmem(msg, item.c_str(), item.size());
    }

    return msg;
  }

  MessageBusMalamute::MessageBusMalamute(const std::string& clientName, const std::string& endpoint)
    : m_clientName(clientName)
    , m_endpoint(endpoint)
  {
    m_publishTopic = "";
    m_syncUuid = "";

    // Create Malamute connection.
    m_client = mlm_client_new();
    if (!m_client)
    {
      throw std::bad_alloc();
    }

    zsys_handler_set(nullptr);
  }

  MessageBusMalamute::~MessageBusMalamute()
  {
    logTrace("{} - destructor", m_clientName);
    zactor_destroy(&m_actor);
    mlm_client_destroy(&m_client);
  }

  ComState MessageBusMalamute::connect()
  {
    auto status = ComState::COM_STATE_NO_CONTACT;
    if (mlm_client_connect(m_client, m_endpoint.c_str(), 1000, m_clientName.c_str()) == -1)
    {
      throw MessageBusException("Failed to connect to Malamute server.");
    }
    status = ComState::COM_STATE_OK;
    logTrace("{} - connected to Malamute server", m_clientName);

    // Create listener thread.
    m_actor = zactor_new(listener, reinterpret_cast<void*>(this));
    if (!m_actor)
    {
      throw std::bad_alloc();
    }
    return status;
  }

  DeliveryState MessageBusMalamute::publish(const std::string& topic, const MlmMessage& message)
  {
    auto delivState = DeliveryState::DELI_STATE_ACCEPTED;
    if (m_publishTopic == "")
    {
      m_publishTopic = topic;
      if (mlm_client_set_producer(m_client, m_publishTopic.c_str()) == -1)
      {
        delivState = DeliveryState::DELI_STATE_REJECTED;
        throw MessageBusException("Failed to set producer on Malamute connection.");
      }
      logTrace("{} - registered as stream producter on '{}'", m_clientName, m_publishTopic);
    }

    if (topic != m_publishTopic)
    {
      delivState = DeliveryState::DELI_STATE_UNKNOWN;
      throw MessageBusException("MessageBusMalamute requires publishing to declared topic.");
    }

    zmsg_t* msg = _toZmsg(message);
    logTrace("{} - publishing on topic '{}'", m_clientName, m_publishTopic);
    mlm_client_send(m_client, topic.c_str(), &msg);
    return delivState;
  }

  DeliveryState MessageBusMalamute::subscribe(const std::string& topic, MessageListener messageListener)
  {
    auto delivState = DeliveryState::DELI_STATE_ACCEPTED;
    if (mlm_client_set_consumer(m_client, topic.c_str(), "") == -1)
    {
      delivState = DeliveryState::DELI_STATE_REJECTED;
      throw MessageBusException("Failed to set consumer on Malamute connection.");
    }

    m_subscriptions.emplace(topic, messageListener);
    logTrace("{} - subscribed to topic '{}'", m_clientName, topic);
    return delivState;
  }

  DeliveryState MessageBusMalamute::unsubscribe(const std::string& topic, MessageListener /*messageListener*/)
  {
    auto delivState = DeliveryState::DELI_STATE_NOT_SUPPORTED;
    auto iterator = m_subscriptions.find(topic);

    if (iterator == m_subscriptions.end())
    {
      delivState = DeliveryState::DELI_STATE_REJECTED;
      throw MessageBusException("Trying to unsubscribe on non-subscribed topic.");
    }

    // Our current Malamute version is too old...
    logWarn("{} - mlm_client_remove_consumer() not implemented", m_clientName);

    m_subscriptions.erase(iterator);
    logTrace("{} - unsubscribed to topic '{}'", m_clientName, topic);
    return delivState;
  }

  DeliveryState MessageBusMalamute::sendRequest(const std::string& requestQueue, const MlmMessage& message)
  {
    std::string to = requestQueue.c_str();
    std::string subject = requestQueue.c_str();

    auto iterator = message.metaData().find(CORRELATION_ID);
    if (iterator == message.metaData().end() || iterator->second == "")
    {
      logWarn("{} - request should have a 'CORRELATION_ID' field", m_clientName.c_str());
    }
    iterator = message.metaData().find(REPLY_TO);
    if (iterator == message.metaData().end() || iterator->second == "")
    {
      logWarn("{} - request should have a 'REPLY_TO' field", m_clientName.c_str());
    }
    iterator = message.metaData().find(TO);
    if (iterator == message.metaData().end() || iterator->second == "")
    {
      logWarn("{} - request should have a 'TO' field", m_clientName.c_str());
    }
    else
    {
      to = iterator->second;
      subject = requestQueue;
    }
    zmsg_t* msg = _toZmsg(message);

    //Todo: Check error code after sendto
    mlm_client_sendto(m_client, to.c_str(), subject.c_str(), nullptr, 200, &msg);
    return DeliveryState::DELI_STATE_ACCEPTED;
  }

  DeliveryState MessageBusMalamute::sendRequest(const std::string& requestQueue, const MlmMessage& message, MessageListener messageListener)
  {
    auto iterator = message.metaData().find(REPLY_TO);
    if (iterator == message.metaData().end() || iterator->second == "")
    {
      throw MessageBusException("Request must have a reply to queue.");
    }
    std::string queue(iterator->second);
    receive(queue, messageListener);
    return sendRequest(requestQueue, message);
  }

  DeliveryState MessageBusMalamute::sendReply(const std::string& replyQueue, const MlmMessage& message)
  {
    auto delivState = DeliveryState::DELI_STATE_ACCEPTED;
    auto iterator = message.metaData().find(CORRELATION_ID);
    if (iterator == message.metaData().end() || iterator->second == "")
    {
      delivState = DeliveryState::DELI_STATE_REJECTED;
      throw MessageBusException("Reply must have a correlation id.");
    }
    iterator = message.metaData().find(TO);
    if (iterator == message.metaData().end() || iterator->second == "")
    {
      logWarn("{} - request should have a 'TO' field", m_clientName.c_str());
    }
    zmsg_t* msg = _toZmsg(message);

    //Todo: Check error code after sendto
    mlm_client_sendto(m_client, iterator->second.c_str(), replyQueue.c_str(), nullptr, 200, &msg);
    return delivState;
  }

  DeliveryState MessageBusMalamute::receive(const std::string& queue, MessageListener messageListener)
  {
    auto delivState = DeliveryState::DELI_STATE_ACCEPTED;
    auto iterator = m_subscriptions.find(queue);
    if (iterator == m_subscriptions.end())
    {
      m_subscriptions.emplace(queue, messageListener);
      logTrace("{} - subscriptions emplaced for queue '{}'", m_clientName.c_str(), queue.c_str());
    }
    else
    {
      logWarn("{} - subscriptions already emplaced for queue '{}'", m_clientName.c_str(), queue.c_str());
    }
    return delivState;
  }

  Opt<MlmMessage> MessageBusMalamute::request(const std::string& requestQueue, const MlmMessage& message, int receiveTimeOut)
  {
    auto replyMsg = Opt<MlmMessage>{};
    auto iterator = message.metaData().find(CORRELATION_ID);

    if (iterator == message.metaData().end() || iterator->second == "")
    {
      throw MessageBusException("Request must have a correlation id.");
    }
    m_syncUuid = iterator->second;
    iterator = message.metaData().find(TO);
    if (iterator == message.metaData().end() || iterator->second == "")
    {
      throw MessageBusException("Request must have a 'TO' field.");
    }

    MlmMessage msg(message);
    // Adding metadata timeout.
    msg.metaData().emplace(TIMEOUT, std::to_string(receiveTimeOut));

    std::unique_lock<std::mutex> lock(m_cv_mtx);
    msg.metaData().emplace(REPLY_TO, m_clientName);
    zmsg_t* msgMlm = _toZmsg(msg);

    //Todo: Check error code after sendto
    logTrace("{} - sending to queue '{}'", m_clientName.c_str(), requestQueue.c_str());
    mlm_client_sendto(m_client, iterator->second.c_str(), requestQueue.c_str(), nullptr, 200, &msgMlm);

    if (m_cv.wait_for(lock, std::chrono::seconds(receiveTimeOut)) != std::cv_status::timeout)
    {
      replyMsg = m_syncResponse;
    }
    return replyMsg;
  }

  void MessageBusMalamute::listener(zsock_t* pipe, void* args)
  {
    MessageBusMalamute* mbm = reinterpret_cast<MessageBusMalamute*>(args);
    mbm->listenerMainloop(pipe);
  }

  void MessageBusMalamute::listenerMainloop(zsock_t* pipe)
  {
    zpoller_t* poller = zpoller_new(pipe, mlm_client_msgpipe(m_client), nullptr);
    zsock_signal(pipe, 0);
    logTrace("{} - listener mainloop ready", m_clientName.c_str());

    bool stopping = false;
    while (!stopping)
    {
      void* which = zpoller_wait(poller, -1);

      if (which == pipe)
      {
        zmsg_t* message = zmsg_recv(pipe);
        char* actor_command = zmsg_popstr(message);
        zmsg_destroy(&message);

        //  $TERM actor command implementation is required by zactor_t interface
        if (streq(actor_command, "$TERM"))
        {
          stopping = true;
          zstr_free(&actor_command);
        }
        else
        {
          logWarn("{} - received '{}' on pipe, ignored", actor_command ? actor_command : "(null)");
          zstr_free(&actor_command);
        }
      }
      else if (which == mlm_client_msgpipe(m_client))
      {
        zmsg_t* message = mlm_client_recv(m_client);
        if (message == nullptr)
        {
          stopping = true;
        }
        else
        {
          const char* subject = mlm_client_subject(m_client);
          const char* from = mlm_client_sender(m_client);
          const char* command = mlm_client_command(m_client);

          if (streq(command, "MAILBOX DELIVER"))
          {
            listenerHandleMailbox(subject, from, message);
          }
          else if (streq(command, "STREAM DELIVER"))
          {
            listenerHandleStream(subject, from, message);
          }
          else
          {
            logError("{} - unknown malamute pattern '{}' from '{}' subject '{}'", m_clientName, command, from, subject);
          }
          zmsg_destroy(&message);
        }
      }
    }

    zpoller_destroy(&poller);

    logDebug("{} - listener mainloop terminated", m_clientName.c_str());
  }

  void MessageBusMalamute::listenerHandleMailbox(const char* subject, const char* from, zmsg_t* message)
  {
    logDebug("{} - received mailbox message from '{}' subject '{}'", m_clientName.c_str(), from, subject);

    auto msg = _fromZmsg(message);

    bool syncResponse = false;
    if (m_syncUuid != "")
    {
      auto it = msg.metaData().find(CORRELATION_ID);
      if (it != msg.metaData().end())
      {
        if (m_syncUuid == it->second)
        {
          std::unique_lock<std::mutex> lock(m_cv_mtx);
          m_syncResponse = msg;
          m_cv.notify_one();
          m_syncUuid = "";
          syncResponse = true;
        }
      }
    }
    if (syncResponse == false)
    {
      auto iterator = m_subscriptions.find(subject);
      if (iterator != m_subscriptions.end())
      {
        try
        {
          (iterator->second)(msg);
        }
        catch (const std::exception& e)
        {
          logError("Error in listener of queue '{}': '{}'", iterator->first, e.what());
        }
        catch (...)
        {
          logError("Error in listener of queue '{}': 'unknown error'", iterator->first);
        }
      }
      else
      {
        logWarn("Message skipped");
      }
    }
  }

  void MessageBusMalamute::listenerHandleStream(const char* subject, const char* from, zmsg_t* message)
  {
    logTrace("{} - received stream message from '{}' subject '{}'", m_clientName.c_str(), from, subject);
    auto msg = _fromZmsg(message);

    auto iterator = m_subscriptions.find(subject);
    if (iterator != m_subscriptions.end())
    {
      try
      {
        (iterator->second)(msg);
      }
      catch (const std::exception& e)
      {
        logError("Error in listener of topic '{}': '{}'", iterator->first, e.what());
      }
      catch (...)
      {
        logError("Error in listener of topic '{}': 'unknown error'", iterator->first);
      }
    }
  }

} // namespace fty::messagebus::mlm
