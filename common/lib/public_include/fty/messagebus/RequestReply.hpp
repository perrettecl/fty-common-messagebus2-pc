/*  =========================================================================
    RequestReply - class description

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

#include "fty/messagebus/MsgListener.hpp"
#include <string>

namespace fty::messagebus
{
  template <typename MessageType>
  class RequestReply
  {
  public:
    RequestReply() = default;
    virtual ~RequestReply() = default;
    virtual void sendRequest(const std::string& requestQueue, const std::string& message, MessageListener<MessageType> messageListener) = 0;
    virtual MessageType sendRequest(const std::string& requestQueue, const std::string& message, int timeOut) = 0;
    virtual void waitRequest(const std::string& requestQueue, MessageListener<MessageType> messageListener) = 0;
    virtual void sendReply(const std::string& response, const MessageType& message) = 0;
  };

} // namespace fty::messagebus
