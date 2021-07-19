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

  enum DeliveryState : uint8_t
  {
    DELI_STATE_UNKNOWN = 0,
    DELI_STATE_ACCEPTED = 1,
    DELI_STATE_REJECTED = 2,
    DELI_STATE_TIMEOUT = 3,
    DELI_STATE_NOT_SUPPORTED = 4,
    DELI_STATE_PENDING = 5,
    DELI_STATE_BUSY = 6,
    DELI_STATE_ABORTED = 7,
    DELI_STATE_UNAVAILABLE = 9,
  };
  std::string to_string(const DeliveryState& state);

} // namespace fty::messagebus
