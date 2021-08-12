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

#include <fty/messagebus/MsgBusMalamute.hpp>

namespace
{
  using MlmMessage = fty::messagebus::mlm::MlmMessage;
  using UserData = fty::messagebus::mlm::UserData;

  static constexpr auto MALAMUTE_IMPL = "Message bus above Malamute implementation";

  // Topic
  static const std::string PREFIX_TOPIC = "t.";

  // Queues
  static const std::string PREFIX_QUEUE = "q.";
  static const std::string PREFIX_REQUEST_QUEUE = PREFIX_QUEUE + "req.";
  static const std::string PREFIX_REPLY_QUEUE = PREFIX_QUEUE + "rep.";
} // namespace

namespace fty::messagebus
{
  MsgBusMalamute::MsgBusMalamute(const ClientName& clientName, const Endpoint& endpoint, const ClientName& destClientName)
    : MessageBus(clientName, endpoint, MALAMUTE_IMPL)
    , m_destClientName(destClientName)
  {
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
    message.metaData().emplace(FROM, clientName());

    return m_msgBus->publish(PREFIX_TOPIC + topic, message);
  }

  DeliveryState MsgBusMalamute::sendRequest(const std::string& requestQueue, const UserData& request, MessageListener<MlmMessage> messageListener)
  {
    assertDestClientName();
    auto message = buildMessage(requestQueue, request);
    m_msgBus->receive(message.metaData().find(REPLY_TO)->second, messageListener);
    return m_msgBus->sendRequest(PREFIX_REQUEST_QUEUE + requestQueue, message);
  }

  Opt<MlmMessage> MsgBusMalamute::sendRequest(const std::string& requestQueue, const UserData& request, int timeOut)
  {
    assertDestClientName();
    return m_msgBus->request(PREFIX_REQUEST_QUEUE + requestQueue, buildMessage(requestQueue, request), timeOut);
  }

  DeliveryState MsgBusMalamute::registerRequestListener(const std::string& requestQueue, MessageListener<MlmMessage> messageListener)
  {
    return m_msgBus->receive(PREFIX_REQUEST_QUEUE + requestQueue, messageListener);
  }

  DeliveryState MsgBusMalamute::sendRequestReply(const MlmMessage& inputRequest, const UserData& response)
  {
    MlmMessage responseMsg;
    responseMsg.userData() = response;
    responseMsg.metaData().emplace(STATUS, STATUS_OK);
    responseMsg.metaData().emplace(SUBJECT, ANSWER_USER_PROPERTY);
    responseMsg.metaData().emplace(FROM, clientName());
    responseMsg.metaData().emplace(TO, inputRequest.metaData().find(FROM)->second);
    responseMsg.metaData().emplace(CORRELATION_ID, inputRequest.metaData().find(CORRELATION_ID)->second);

    return m_msgBus->sendReply(inputRequest.metaData().find(REPLY_TO)->second, responseMsg);
  }

  ClientName MsgBusMalamute::destClientName() const
  {
    return m_destClientName;
  }

  void MsgBusMalamute::destClientName(const ClientName& destClientName)
  {
    m_destClientName = destClientName;
  }

  void MsgBusMalamute::assertDestClientName()
  {
    if (destClientName().empty())
    {
      throw MessageBusException("The Malamute destination name is not set, the request can not be send!");
    }
  }

  MlmMessage MsgBusMalamute::buildMessage(const std::string& queue, const UserData& msg)
  {
    auto correlationId = utils::generateUuid();
    auto replyTo = PREFIX_REPLY_QUEUE + queue;

    MlmMessage message;
    message.userData() = msg;
    message.metaData().clear();
    message.metaData().emplace(SUBJECT, QUERY_USER_PROPERTY);
    message.metaData().emplace(FROM, clientName());
    message.metaData().emplace(TO, destClientName());
    message.metaData().emplace(REPLY_TO, replyTo);
    message.metaData().emplace(CORRELATION_ID, correlationId);

    return message;
  }

} // namespace fty::messagebus
