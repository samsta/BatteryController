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
   CanPort(const char* name, int epoll_fd);
   ~CanPort();

   void setSink(can::FrameSink& sink);

   void setupLogger(
         CPlusPlusLogging::Logger& log,
         const char* logger_prefix = "",
         const char* logger_color = nullptr);

   virtual void sink(const can::DataFrame& f);
private:
   virtual void handle();

   int             m_epoll_fd;
   int             m_fd;
   std::string     m_name;
   can::FrameSink* m_sink;

   CPlusPlusLogging::Logger* m_log;
   std::string       m_log_prefix;
   std::string       m_log_color;
   std::string       m_log_color_reset;
};

}

#endif /* _CORE_SOCKETCAN_CANPORT_HPP_ */
