/*  =========================================================================
    MsgBusMqttRequestReply - class description

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

#include "fty/messagebus/mqtt/MsgBusMqttRequestReply.hpp"

namespace fty::messagebus::mqttv5
{
  void MqttRequestReply::sendRequest(const std::string& requestQueue, const std::string& request, MessageListener messageListener)
  {
    auto message = buildMessage(requestQueue, request);
    m_msgBus->receive(message.metaData().find(REPLY_TO)->second, messageListener);
    m_msgBus->sendRequest(PREFIX_REQUEST_QUEUE + requestQueue, message);
  }

  Message MqttRequestReply::sendRequest(const std::string& requestQueue, const std::string& request, int timeOut)
  {
    return m_msgBus->request(PREFIX_REQUEST_QUEUE + requestQueue, buildMessage(requestQueue, request), timeOut);
  }

  void MqttRequestReply::sendReply(const std::string& response, const Message& message)
  {
    Message responseMsg;
    responseMsg.userData() = response;
    responseMsg.metaData().emplace(STATUS, STATUS_OK);
    responseMsg.metaData().emplace(SUBJECT, ANSWER_USER_PROPERTY);
    responseMsg.metaData().emplace(FROM, m_clientName);
    responseMsg.metaData().emplace(REPLY_TO, message.metaData().find(REPLY_TO)->second);
    responseMsg.metaData().emplace(CORRELATION_ID, message.metaData().find(CORRELATION_ID)->second);

    m_msgBus->sendReply(message.metaData().find(REPLY_TO)->second, responseMsg);
  }

  void MqttRequestReply::waitRequest(const std::string& requestQueue, MessageListener messageListener)
  {
    m_msgBus->receive(PREFIX_REQUEST_QUEUE + requestQueue, messageListener);
  }

  Message MqttRequestReply::buildMessage(const std::string& queue, const std::string& request)
  {
    auto correlationId = utils::generateUuid();
    auto replyTo = PREFIX_REPLY_QUEUE + queue + '/' + correlationId;

    Message message;
    message.userData() = request;
    message.metaData().clear();
    message.metaData().emplace(SUBJECT, QUERY_USER_PROPERTY);
    message.metaData().emplace(FROM, m_clientName);
    message.metaData().emplace(REPLY_TO, replyTo);
    message.metaData().emplace(CORRELATION_ID, correlationId);

    return message;
  }

} // namespace fty::messagebus::mqttv5
