/*  =========================================================================
    fty_common_messagebus_interface - class description

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


#include <functional>
#include <string>

namespace fty::messagebus
{

  enum class ReturnStatus
  {
    MALAMUTE,
    MQTT
  };

  template <typename MessageType>
  using MessageListener = std::function<void(MessageType)>;

  template <typename MessageType>
  class IMessageBus
  {
  public:
    virtual ~IMessageBus() = default;

    /**
     * @brief Try a connection with message bus
     *
     *
     * @throw MessageBusException any exceptions
     */
    virtual void connect() = 0;

    /**
     * @brief Publish message to a topic
     *
     * @param topic     The topic to use
     * @param message   The message object to send
     *
     * @throw MessageBusException any exceptions
     */
    virtual void publish(const std::string& topic, const MessageType& message) = 0;

    /**
     * @brief Subscribe to a topic
     *
     * @param topic             The topic to subscribe
     * @param messageListener   The message listener to call on message
     *
     * @throw MessageBusException any exceptions
     */
    virtual void subscribe(const std::string& topic, MessageListener<MessageType> messageListener) = 0;

    /**
     * @brief Unsubscribe to a topic
     *
     * @param topic             The topic to unsubscribe
     * @param messageListener   The message listener to remove from topic
     *
     * @throw MessageBusException any exceptions
     */
    virtual void unsubscribe(const std::string& topic, MessageListener<MessageType> messageListener) = 0;

    /**
     * @brief Send request to a queue
     *
     * @param requestQueue    The queue to use
     * @param message         The message to send
     *
     * @throw MessageBusException any exceptions
     */
    virtual void sendRequest(const std::string& requestQueue, const MessageType& message) = 0;

    /**
     * @brief Send request to a queue and receive response to a specific listener
     *
     * @param requestQueue    The queue to use
     * @param message         The message to send
     * @param messageListener The listener where to receive response (on queue set to reply to field)
     *
     * @throw MessageBusException any exceptions
     */
    virtual void sendRequest(const std::string& requestQueue, const MessageType& message, MessageListener<MessageType> messageListener) = 0;

    /**
     * @brief Send a reply to a queue
     *
     * @param replyQueue      The queue to use
     * @param message         The message to send
     *
     * @throw MessageBusException any exceptions
     */
    virtual void sendReply(const std::string& replyQueue, const MessageType& message) = 0;

    /**
     * @brief Receive message from queue
     *
     * @param queue             The queue where receive message
     * @param messageListener   The message listener to use for this queue
     *
     * @throw MessageBusException any exceptions
     */
    virtual void receive(const std::string& queue, MessageListener<MessageType> messageListener) = 0;

    /**
     * @brief Send request to a queue and wait to receive response
     *
     * @param requestQueue    The queue to use
     * @param message         The message to send
     * @param receiveTimeOut  Wait for response until timeout is reach
     *
     * @return message as response
     *
     * @throw MessageBusException any exceptions
     */
    virtual MessageType request(const std::string& requestQueue, const MessageType& message, int receiveTimeOut) = 0;

  protected:
    IMessageBus() = default;
  };

} // namespace fty::messagebus
