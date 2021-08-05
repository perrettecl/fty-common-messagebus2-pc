/*  =========================================================================
    FtyCommonMqttTestDef.hpp - class description

    Copyright (C) 2014 - 2021 Eaton

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

namespace fty::messagebus::mlm::test
{
  // Client name shared by all sample
  static auto constexpr SHARED_CLIENT_NAME = "sampleReqRep";

  // Queue name shared by multies samples
  static auto constexpr QUEUE_NAME = "doAction";

} // namespace fty::messagebus::mlm::test
