/*  =========================================================================
    MsgBusMqtt.hpp - class description

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

#include <fty/messagebus/MsgBusWrapper.hpp>
#include <fty/messagebus/mqtt/MsgBusMqtt.hpp>
#include <fty/messagebus/utils/MsgBusHelper.hpp>

namespace fty::messagebus
{
  class MsgBusMqtt : public MsgBusWrapper<mqttv5::MessageBusMqtt, mqttv5::MqttMessage, mqttv5::UserData>
  {
  public:
    MsgBusMqtt(const ClientName& clientName = utils::getClientId("MsgBusMqtt"), const Endpoint& endpoint = mqttv5::DEFAULT_MQTT_END_POINT);
    ~MsgBusMqtt() = default;

    MsgBusMqtt(MsgBusMqtt && other) = default;
    MsgBusMqtt& operator=(MsgBusMqtt&& other) = delete;
    MsgBusMqtt(const MsgBusMqtt& other) = default;
    MsgBusMqtt& operator=(const MsgBusMqtt& other) = delete;

    DeliveryState subscribe(const std::string& topic, MessageListener<mqttv5::MqttMessage> messageListener) override;
    DeliveryState unsubscribe(const std::string& topic) override;
    DeliveryState publish(const std::string& topic, const mqttv5::UserData& msg) override;

    DeliveryState sendRequest(const std::string& requestQueue, const mqttv5::UserData& msg, MessageListener<mqttv5::MqttMessage> messageListener) override;
    Opt<mqttv5::MqttMessage> sendRequest(const std::string& requestQueue, const mqttv5::UserData& msg, int timeOut) override;
    DeliveryState registerRequestListener(const std::string& requestQueue, MessageListener<mqttv5::MqttMessage> messageListener) override;
    DeliveryState sendRequestReply(const mqttv5::MqttMessage& inputRequest, const mqttv5::UserData& response) override;

  protected:

    mqttv5::MqttMessage buildMessage(const std::string& queue, const mqttv5::UserData& msg) override;
  };
} // namespace fty::messagebus
