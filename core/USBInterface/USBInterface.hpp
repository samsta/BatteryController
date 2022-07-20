/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CORE_USBINTERFACE_USBINTERFACE_HPP_
#define CORE_USBINTERFACE_USBINTERFACE_HPP_

#include "can/FrameSink.hpp"
#include "core/Linux/EpollHandler.hpp"
#include "logging/stream.hpp"
#include <string>

namespace core
{

class USBPort: private core::EpollHandler, public can::FrameSink
{
public:
   USBPort(const char* name, int epoll_fd);
   ~USBPort();

   void setSink(can::FrameSink& sink);

   void setupLogger(
         logging::ostream& log,
         const char* logger_prefix = "",
         const char* logger_color = nullptr);

   virtual void sink(const can::DataFrame& f);
private:
   virtual void handle();

   int             m_epoll_fd;
   int             m_fd;
   std::string     m_name;
   can::FrameSink* m_sink;

   logging::ostream* m_log;
   std::string       m_log_prefix;
   std::string       m_log_color;
   std::string       m_log_color_reset;
};

}

#endif /* CORE_USBINTERFACE_USBINTERFACE_HPP_ */
