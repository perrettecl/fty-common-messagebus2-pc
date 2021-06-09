/*  =========================================================================
    fty_common_messagebus_message - class description

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

#ifndef FTY_COMMON_MESSAGEBUS_IMESSAGE_H
#define FTY_COMMON_MESSAGEBUS_IMESSAGE_H

#include <list>
#include <map>
#include <string>

namespace messagebus
{

  using MetaData = std::map<std::string, std::string>;

  static constexpr auto STATUS_OK = "ok";
  static constexpr auto STATUS_KO = "ko";

  // Metadata user property
  static constexpr auto QUERY_USER_PROPERTY = "_query";
  static constexpr auto ANSWER_USER_PROPERTY = "_answer";

  static constexpr auto REPLY_TO = "_replyTo";
  static constexpr auto CORRELATION_ID = "_correlationId";
  static constexpr auto FROM = "_from";
  static constexpr auto TO = "_to";
  static constexpr auto SUBJECT = "_subject";
  static constexpr auto STATUS = "_status";
  static constexpr auto TIMEOUT = "_timeout";
  static constexpr auto META_DATA = "metaData";
  static constexpr auto USER_DATA = "userData";

  template <typename T>
  class IMessage
  {
  public:
    IMessage() = default;
    IMessage(const MetaData& metaData, const T& userData)
      : m_metadata(metaData)
      , m_data(userData){};

    virtual ~IMessage() noexcept = default;

    MetaData& metaData()
    {
      return m_metadata;
    }

    T& userData()
    {
      return m_data;
    }

    const MetaData& metaData() const
    {
      return m_metadata;
    }

    const T& userData() const
    {
      return m_data;
    }

  protected:
    MetaData m_metadata;
    T m_data;
  };

} // namespace messagebus

#endif // FTY_COMMON_MESSAGEBUS_IMESSAGE_H
