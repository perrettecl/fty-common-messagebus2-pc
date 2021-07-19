/*  =========================================================================
    MsgBusMqttPublishSubscribe - class description

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

#include "fty/messagebus/mqtt/MsgBusMqttPublishSubscribe.hpp"

namespace fty::messagebus::mqttv5
{
  DeliveryState MsgBusMqttPublishSubscribe::subscribe(const std::string& topic, MessageListener messageListener)
  {
    return m_msgBus->subscribe(PREFIX_TOPIC + topic, messageListener);
  }

  DeliveryState MsgBusMqttPublishSubscribe::unsubscribe(const std::string& topic)
  {
    return m_msgBus->unsubscribe(PREFIX_TOPIC + topic, nullptr);
  }

  DeliveryState MsgBusMqttPublishSubscribe::publish(const std::string& topic, const std::string& msg)
  {
    Message message;
    message.userData() = msg;
    message.metaData().clear();
    message.metaData().emplace(SUBJECT, PUBLISH_USER_PROPERTY);
    message.metaData().emplace(FROM, m_clientName);

    return m_msgBus->publish(PREFIX_TOPIC + topic, message);
  }

} // namespace fty::messagebus::mqttv5
