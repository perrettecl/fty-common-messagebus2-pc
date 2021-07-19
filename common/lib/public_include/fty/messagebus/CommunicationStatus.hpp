/*  =========================================================================
    CommunicationStatus - class description

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

#include <string>

namespace fty::messagebus
{
  enum ComState : uint8_t
  {
    COM_STATE_UNKNOWN = 0,
    COM_STATE_NONE = 1,
    COM_STATE_OK = 2,
    COM_STATE_LOST = 3,
    COM_STATE_NO_CONTACT = 4,
    COM_STATE_CONNECT_FAILED = 5,
  };
  std::string to_string(const ComState& state);

  struct CommunicationStatus
  {
    ComState state;
    CommunicationStatus();
    explicit CommunicationStatus(const ComState comState);
    CommunicationStatus(const CommunicationStatus& other);

    bool set(const ComState comState);

    bool operator==(const CommunicationStatus& other) const;
    bool operator!=(const CommunicationStatus& other) const;
  };
  std::ostream& operator<<(std::ostream& os, const CommunicationStatus& status);

} // namespace fty::messagebus
