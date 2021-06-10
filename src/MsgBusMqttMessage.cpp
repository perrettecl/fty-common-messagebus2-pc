/*  =========================================================================
    fty_common_messagebus_mqtt_message - class description

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

#include "fty/messagebus/mqtt/MsgBusMqttMessage.hpp"

#include <fty_log.h>
#include <jsoncpp/json/json.h>

namespace fty::messagebus::mqttv5
{
  MqttMessage::MqttMessage(const MetaData& metaData, const UserData& userData)
  {
    m_metadata = metaData;
    m_data = userData;
  }

  MqttMessage::MqttMessage(const MetaData& metaData, const std::string& input)
  {
    m_metadata = metaData;
    deSerialize(input);
  }

  auto MqttMessage::serialize() const -> std::string const
  {
    // user values
    Json::Value userValues(Json::arrayValue);
    // Iterate over all user values.
    for (const auto& value : m_data)
    {
      userValues.append(value);
    }
    return Json::writeString(Json::StreamWriterBuilder{}, userValues);
  }

  void MqttMessage::deSerialize(const std::string& input)
  {
    Json::Value root;
    Json::Reader reader;
    bool parsingStatus = reader.parse(input.c_str(), root);
    if (!parsingStatus)
    {
      log_error("Failed on parsing: %s", reader.getFormattedErrorMessages().c_str());
    }
    else
    {
      // User data
      const Json::Value& userDataArray = root;
      for (unsigned int i = 0; i < userDataArray.size(); i++)
      {
        m_data.push_back(userDataArray[i].asString());
      }
    }
  }

} // namespace messagebus::mqttv5
