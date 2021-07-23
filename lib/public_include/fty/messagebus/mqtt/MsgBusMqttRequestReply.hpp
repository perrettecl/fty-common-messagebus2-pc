/*  =========================================================================
    MsgBusMqttRequestReply - class description

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

#pragma once

#include "fty/messagebus/mqtt/MsgBusMqttWrapper.hpp"
#include <fty/messagebus/RequestReply.hpp>
#include <fty/messagebus/mqtt/MsgBusMqttDef.hpp>

namespace fty::messagebus::mqttv5
{
  class [[nodiscard]] MqttRequestReply final : public fty::messagebus::mqttv5::MsgBusMqttWrapper, fty::messagebus::RequestReply<Message>
  {
  public:
    MqttRequestReply(const std::string& endpoint = DEFAULT_MQTT_END_POINT, const std::string& clientName = utils::getClientId("MqttRequestReply"))
      : MsgBusMqttWrapper(endpoint, clientName){};

    DeliveryState sendRequest(const std::string& requestQueue, const std::string& message, MessageListener messageListener) override;
    Opt<Message> sendRequest(const std::string& requestQueue, const std::string& message, int timeOut) override;
    DeliveryState waitRequest(const std::string& requestQueue, MessageListener messageListener) override;
    DeliveryState sendReply(const std::string& response, const Message& message) override;

  private:
    Message buildMessage(const std::string& queue, const std::string& request);
  };

} // namespace fty::messagebus::mqttv5
