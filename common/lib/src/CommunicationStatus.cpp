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

#include "fty/messagebus/CommunicationStatus.hpp"

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

   CommunicationStatus::CommunicationStatus()
    : CommunicationStatus(COM_STATE_NO_CONTACT)
  {
  }

  CommunicationStatus::CommunicationStatus(const ComState comState)
    : state(comState)
  {
  }

  CommunicationStatus::CommunicationStatus(const CommunicationStatus& other)
    : CommunicationStatus(other.state)
  {
  }

  bool CommunicationStatus::set(const ComState comState)
  {
    bool differs = (state != comState);
    if (differs)
    {
      state = comState;
    }
    return differs;
  }

  bool CommunicationStatus::operator==(const CommunicationStatus& other) const
  {
    return (state == other.state);
  }

  bool CommunicationStatus::operator!=(const CommunicationStatus& other) const
  {
    return !operator==(other);
  }

  std::ostream& operator<<(std::ostream& os, const CommunicationStatus& status)
  {
    os << "communication status:" << std::endl;
    os << "\t\tstate = " << status.state << std::endl;
    return os;
  }

} // namespace fty::messagebus
