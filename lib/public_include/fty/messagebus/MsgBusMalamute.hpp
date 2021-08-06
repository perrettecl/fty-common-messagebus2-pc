/*  =========================================================================
    MsgBusMalamute.hpp - class description

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

#include <fty/messagebus/MsgBusWrapper.hpp>
#include <fty/messagebus/mlm/MsgBusMalamute.hpp>
#include <fty/messagebus/utils/MsgBusHelper.hpp>

namespace fty::messagebus
{
  class MsgBusMalamute : public MsgBusWrapper<mlm::MessageBusMalamute, mlm::MlmMessage, mlm::UserData>
  {
  public:
    MsgBusMalamute(const ClientName& clientName = utils::getClientId("MsgBusMalamute"), const Endpoint& endpoint = fty::messagebus::mlm::DEFAULT_MLM_END_POINT);
    ~MsgBusMalamute() = default;

    MsgBusMalamute(MsgBusMalamute && other) = default;
    MsgBusMalamute& operator=(MsgBusMalamute&& other) = delete;
    MsgBusMalamute(const MsgBusMalamute& other) = default;
    MsgBusMalamute& operator=(const MsgBusMalamute& other) = delete;

    DeliveryState subscribe(const std::string& topic, MessageListener<mlm::MlmMessage> messageListener) override;
    DeliveryState unsubscribe(const std::string& topic) override;
    DeliveryState publish(const std::string& topic, const mlm::UserData& msg) override;

    DeliveryState sendRequest(const std::string& requestQueue, const mlm::UserData& msg, MessageListener<mlm::MlmMessage> messageListener) override;
    DeliveryState sendRequest(const std::string& destName, const std::string& requestQueue, const mlm::UserData& msg, MessageListener<mlm::MlmMessage> messageListener);
    Opt<mlm::MlmMessage> sendRequest(const std::string& requestQueue, const mlm::UserData& msg, int timeOut) override;
    Opt<mlm::MlmMessage> sendRequest(const std::string& destName, const std::string& requestQueue, const mlm::UserData& msg, int timeOut);
    DeliveryState registerRequestListener(const std::string& requestQueue, MessageListener<mlm::MlmMessage> messageListener) override;
    DeliveryState sendRequestReply(const mlm::MlmMessage& inputRequest, const mlm::UserData& response) override;

    ClientName destClientName() const
    {
      return m_destClientName;
    };

    void destClientName(const ClientName& destClientName)
    {
      m_destClientName = destClientName;
    };

  private:

    ClientName m_destClientName{};
    void assertDestClientName();

  protected:
    mlm::MlmMessage buildMessage(const std::string& queue, const mlm::UserData& msg) override;
  };
} // namespace fty::messagebus
