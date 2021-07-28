/*  =========================================================================
    MsgBusMalamute.cpp - class description

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

#include "fty/messagebus/MsgBusMalamute.hpp"

namespace
{
  // Topic
  static const std::string PREFIX_TOPIC = "/etn/t";

  // Queues
  static const std::string PREFIX_QUEUE = "/etn/q/";
  static const std::string PREFIX_REQUEST_QUEUE = PREFIX_QUEUE + "request";
  static const std::string PREFIX_REPLY_QUEUE = PREFIX_QUEUE + "reply";
} // namespace

namespace fty::messagebus
{
  using MlmMessage = fty::messagebus::mlm::MlmMessage;
  using UserData = fty::messagebus::mlm::UserData;

  static constexpr auto MALAMUTE_IMPL = "Message bus above Malamute implementation";

  MsgBusMalamute::MsgBusMalamute(const ClientName& clientName, const Endpoint& endpoint)
    : MsgBusWrapper(clientName, endpoint)
  {
  }

  std::string MsgBusMalamute::identify() const
  {
    return MALAMUTE_IMPL;
  }

  DeliveryState MsgBusMalamute::subscribe(const std::string& topic, MessageListener<MlmMessage> messageListener)
  {
    return m_msgBus->subscribe(PREFIX_TOPIC + topic, messageListener);
  }

  DeliveryState MsgBusMalamute::unsubscribe(const std::string& topic)
  {
    return m_msgBus->unsubscribe(PREFIX_TOPIC + topic, nullptr);
  }

  DeliveryState MsgBusMalamute::publish(const std::string& topic, const UserData& msg)
  {
    MlmMessage message;
    message.userData() = msg;
    message.metaData().clear();
    message.metaData().emplace(SUBJECT, PUBLISH_USER_PROPERTY);
    message.metaData().emplace(FROM, m_clientName);

    return m_msgBus->publish(PREFIX_TOPIC + topic, message);
  }

  DeliveryState MsgBusMalamute::sendRequest(const std::string& requestQueue, const UserData& request, MessageListener<MlmMessage> messageListener)
  {
    auto message = buildMessage(requestQueue, request);
    m_msgBus->receive(message.metaData().find(REPLY_TO)->second, messageListener);
    return m_msgBus->sendRequest(PREFIX_REQUEST_QUEUE + requestQueue, message);
  }

  Opt<MlmMessage> MsgBusMalamute::sendRequest(const std::string& requestQueue, const UserData& msg, int timeOut)
  {
    return m_msgBus->request(PREFIX_REQUEST_QUEUE + requestQueue, buildMessage(requestQueue, msg), timeOut);
  }

  DeliveryState MsgBusMalamute::waitRequest(const std::string& requestQueue, MessageListener<MlmMessage> messageListener)
  {
    return m_msgBus->receive(PREFIX_REQUEST_QUEUE + requestQueue, messageListener);
  }

  DeliveryState MsgBusMalamute::sendReply(const UserData& response, const MlmMessage& message)
  {
    MlmMessage responseMsg;
    responseMsg.userData() = response;
    responseMsg.metaData().emplace(STATUS, STATUS_OK);
    responseMsg.metaData().emplace(SUBJECT, ANSWER_USER_PROPERTY);
    responseMsg.metaData().emplace(FROM, m_clientName);
    responseMsg.metaData().emplace(REPLY_TO, message.metaData().find(REPLY_TO)->second);
    responseMsg.metaData().emplace(CORRELATION_ID, message.metaData().find(CORRELATION_ID)->second);

    return m_msgBus->sendReply(message.metaData().find(REPLY_TO)->second, responseMsg);
  }

  MlmMessage MsgBusMalamute::buildMessage(const std::string& queue, const UserData& msg)
  {
    auto correlationId = utils::generateUuid();
    auto replyTo = PREFIX_REPLY_QUEUE + queue + '/' + correlationId;

    MlmMessage message;
    message.userData() = msg;
    message.metaData().clear();
    message.metaData().emplace(SUBJECT, QUERY_USER_PROPERTY);
    message.metaData().emplace(FROM, m_clientName);
    message.metaData().emplace(REPLY_TO, replyTo);
    message.metaData().emplace(CORRELATION_ID, correlationId);

    return message;
  }
} // namespace fty::messagebus
