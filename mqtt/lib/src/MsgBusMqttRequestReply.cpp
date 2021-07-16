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

/*
@header
    fty_common_messagebus_mqtt_message -
@discuss
@end
*/

#include "fty/messagebus/mqtt/MsgBusMqttRequestReply.hpp"

#include <cxxabi.h>
#include <iostream>

namespace fty::messagebus::mqttv5
{

  // MqttRequestReply::MqttRequestReply(const std::string& endpoint, const std::string& clientName)
  // {
  //   // m_msgBus = std::unique_ptr<IMessageBus<Message>> {fty::messagebus::MessageBusFactory::createMqttMsgBus(endpoint, clientName)};
  //   // //static_assert(std::is_same_v<std::unique_ptr<IMessageBus<Message>>, decltype(fty::messagebus::MessageBusFactory::createMqttMsgBus(endpoint, clientName))>);
  //   // //std::unique_ptr<IMessageBus<Message>> msgBus = fty::messagebus::MessageBusFactory::createMqttMsgBus(endpoint, clientName);
  //   // // m_msgBus = fty::messagebus::MessageBusFactory::createMqttMsgBus(endpoint, clientName);
  //   /*std::unique_ptr<IMessageBus<Message>>*/
  //   auto msgBus = MessageBusFactory::createMqttMsgBus(endpoint, clientName);
  //   int status;
  //   char * policyType = abi::__cxa_demangle(typeid(msgBus).name(), 0, 0, &status);

  //   std::cout << "msgBus => "<< policyType << std::endl;

  //   std::cout << "msgBus => "<< abi::__cxa_demangle(typeid(std::unique_ptr<IMessageBus<Message>>).name(), 0, 0, &status) << std::endl;
  //   std::cout << "msgBus => "<< abi::__cxa_demangle(typeid(decltype(fty::messagebus::MessageBusFactory::createMqttMsgBus(endpoint, clientName))).name(), 0, 0, &status) << std::endl;

  //   //std::make_unique<session::Authorization>(std::move(authorization))
  //   //m_msgBus = std::make_unique<Message>(std::move(endpoint), std::move(clientName));
  //   //   m_msgBus->connect();
  // }

  void MqttRequestReply::onMessage(const Message& /*msg*/)
  {
  }

  void MqttRequestReply::sendRequest(const std::string& requestQueue, const Message& message /*, MessageListener messageListener*/)
  {
    auto correlationId = utils::generateUuid();
    auto replyTo = "/etn/q/reply" + '/' + utils::generateUuid();

    //requester->receive(replyTo, messageListener);
    m_msgBus->sendRequest(requestQueue, message);
  }

} // namespace fty::messagebus::mqttv5
