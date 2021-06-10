/*  =========================================================================
    fty_common_messagebus_example_rep2 - Provides message bus for agents

    Copyright (C) 2019 - 2020 Eaton

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

/*! \file   fty_common_messagebus_example_rep2.cc
    \brief  Provides message bus for agents - example
    \author Jean-Baptiste Boric <Jean-BaptisteBORIC@Eaton.com>
    \author Xavier Millieret <XavierMillieret@eaton.com>
    \author Clement Perrette <clementperrette@eaton.com>
*/

#include "FtyCommonMessageBusDto.hpp"
#include "fty_common_messagebus_Imessage.hpp"
#include "fty_common_messagebus_exception.h"
#include "fty_common_messagebus_factory.hpp"
#include "MsgBusHelper.hpp"

#include <fty_log.h>

using namespace std::placeholders;
using namespace fty::messagebus;
using namespace fty::messagebus::mlm;

namespace srr
{
  class SrrManager
  {
  public:
    SrrManager();
    ~SrrManager();
    void init();
    void handleRequest(const MlmMessage& msg);
    MessageBusMalamute* m_msgBus;
  };

  // All define value
  const int DEFAULT_TIME_OUT = 10;
  const char* DATA = "data";

  /**
 *
 * @param parameters
 * @param streamPublisher
 */
  SrrManager::SrrManager()
  {
    init();
  }

  SrrManager::~SrrManager()
  {
    log_debug("Delete all Srr resources");
    if (m_msgBus)
    {
      delete m_msgBus;
      log_debug("msgBus resource deleted");
    }
  }

  /**
 *
 */
  void SrrManager::init()
  {
    try
    {
      // Message bus init
      m_msgBus = MessagebusFactory::createMlmMsgBus(DEFAULT_MLM_END_POINT, "receiver");
      m_msgBus->connect();

      log_info("messagebus::connect");
      // Listen all incoming request
      // messagebus::Message fct = [&](messagebus::Message msg){this->handleRequest(msg);};
      auto fct = std::bind(&SrrManager::handleRequest, this, _1);
      m_msgBus->receive("doAction.queue.query", fct);
      log_info("m_msgBus->receive");
    }
    catch (MessageBusException& ex)
    {
      log_error("Message bus error: %s", ex.what());
    }
    catch (...)
    {
      log_error("Unexpected error: unknown");
    }
  }

  /**
 *
 * @param sender
 * @param payloadea
 * @return
 */
  void SrrManager::handleRequest(const MlmMessage& message)
  {
    log_info("queryListener:");
    for (const auto& pair : message.metaData())
    {
      log_info("  ** '%s' : '%s'", pair.first.c_str(), pair.second.c_str());
    }
    UserData data = message.userData();
    FooBar fooBar;
    data >> fooBar;
    log_info("  * foo    : '%s'", fooBar.foo.c_str());
    log_info("  * bar    : '%s'", fooBar.bar.c_str());

    if (message.metaData().size() != 0)
    {
      MlmMessage response;
      MetaData metadata;
      auto fooBarr = FooBar("status::ok", fooBar.bar.c_str());
      UserData data2;
      data2 << fooBarr;
      response.userData() = data2;
      response.metaData().emplace(SUBJECT, "response");
      response.metaData().emplace(TO, message.metaData().find(FROM)->second);
      response.metaData().emplace(CORRELATION_ID, message.metaData().find(CORRELATION_ID)->second);
      m_msgBus->sendReply(message.metaData().find(REPLY_TO)->second, response);
    }
    else
    {
      log_info("Old format, skip query...");
    }
  }
} // namespace srr

volatile bool _continue = true;

void my_handler(int s)
{
  printf("Caught signal %d\n", s);
  _continue = false;
}

int main(int /*argc*/, char** argv)
{
  log_info(argv[0]);

  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = my_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  srr::SrrManager manager;
  do
  {

    std::this_thread::sleep_for(std::chrono::seconds(1));

  } while (_continue == true);

  log_info(argv[0]);

  return EXIT_SUCCESS;
}
