/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CORE_SOCKETCAN_CANPORT_HPP_
#define _CORE_SOCKETCAN_CANPORT_HPP_

#include "can/FrameSink.hpp"
#include "core/Linux/EpollHandler.hpp"
#include "logging/logging.hpp"
#include <string>

namespace core
{

class CanPort: private core::EpollHandler, public can::FrameSink
{
public:
   CanPort(const char* name, int epoll_fd, logging::Logger *log);
   ~CanPort();

   void setSink(can::FrameSink& sink);
   virtual void sink(const can::DataFrame& f);

private:
   virtual void handle();

   int             m_epoll_fd;
   int             m_fd;
   std::string     m_name;
   can::FrameSink* m_sink;

   logging::Logger* m_log;
};

}

#endif /* _CORE_SOCKETCAN_CANPORT_HPP_ */
