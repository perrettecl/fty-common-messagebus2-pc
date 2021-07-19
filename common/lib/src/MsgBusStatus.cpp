/*  =========================================================================
    CommunicationStatus - class implementation

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

#include "fty/messagebus/MsgBusStatus.hpp"

#include <ostream>
#include <sstream>

namespace fty::messagebus
{
  std::string to_string(const ComState& state)
  {
    switch (state)
    {
      case COM_STATE_UNKNOWN:
        return "UNKNOWN";
      case COM_STATE_NONE:
        return "NONE";
      case COM_STATE_OK:
        return "OK";
      case COM_STATE_LOST:
        return "LOST";
      case COM_STATE_NO_CONTACT:
        return "NO CONTACT";
      case COM_STATE_CONNECT_FAILED:
        return "CONNECTION FAILED";
      default:
        break;
    }
    return "UNDEFINED";
  }

  std::string to_string(const DeliveryState& state)
  {
    switch (state)
    {
      case DELI_STATE_UNKNOWN:
        return "UNKNOWN";
      case DELI_STATE_ACCEPTED:
        return "ACCEPTED";
      case DELI_STATE_REJECTED:
        return "REJECTED";
      case DELI_STATE_TIMEOUT:
        return "TIMEOUT";
      case DELI_STATE_NOT_SUPPORTED:
        return "NOT SUPPORTED";
      case DELI_STATE_PENDING:
        return "PENDING";
      case DELI_STATE_BUSY:
        return "BUSY";
      case DELI_STATE_ABORTED:
        return "ABORTED";
      default:
        break;
    }
    return "UNDEFINED";
  }



} // namespace fty::messagebus
