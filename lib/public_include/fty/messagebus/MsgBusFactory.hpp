/*  =========================================================================
    MsgBusFactory.hpp - class description

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

#include <memory>
#include <string>

namespace fty::messagebus
{
  template <typename MessageType>
  class MessageBusFactory
  {

  public:
    MessageBusFactory() = delete;
    virtual ~MessageBusFactory(){};

    /**
    * @brief Create a Message bus
    *
    * @param _endpoint Message bus end point
    * @param clientName prefix for client Name
    *
    * @return std::unique_ptr<MessageType>
    */
    static std::unique_ptr<MessageType> createMsgBus(const std::string& _endpoint, const std::string& _clientName)
    {
      return std::make_unique<MessageType>(_endpoint, _clientName);
    };
  };

} // namespace fty::messagebus
