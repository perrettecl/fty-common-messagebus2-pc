/*  =========================================================================
    fty_common_messagebus_mlm_message - class description

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

#ifndef FTY_COMMON_MESSAGEBUS_MLM_MESSAGE_HPP
#define FTY_COMMON_MESSAGEBUS_MLM_MESSAGE_HPP

#include "fty_common_messagebus_Imessage.hpp"

#include <list>

namespace fty::messagebus
{
  // Json representation
  using UserData = std::list<std::string>;

  class MlmMessage final : public IMessage<UserData>
  {
  public:
    MlmMessage() = default;
    MlmMessage(const MetaData& metaData, const UserData& userData = {});
    ~MlmMessage() = default;
  };

} // namespace fty::messagebus::mlm

#endif // FTY_COMMON_MESSAGEBUS_MLM_MESSAGE_HPP
