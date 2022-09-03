/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CORE_USBINTERFACE_USBINTERFACE_HPP_
#define CORE_USBINTERFACE_USBINTERFACE_HPP_

#include "can/FrameSink.hpp"
#include "core/Linux/EpollHandler.hpp"
#include "logging/stream.hpp"
#include <string>

namespace core
{

class USBPort: private core::EpollHandler // , public can::FrameSink
{
public:
   USBPort(const char* name, int epoll_fd);
   ~USBPort();

   void setSinkInbound_1(can::FrameSink& sink);
   void setSinkInbound_2(can::FrameSink& sink);

   can::FrameSink& getSinkOutbound_1();
//   can::FrameSink& getOutboundSink_2();

   void setupLogger(
         logging::ostream& log,
         const char* logger_prefix = "",
         const char* logger_color = nullptr);

   void sinkOutbound_1(const can::DataFrame& f);
//   virtual void sinkOutbound_1(const can::DataFrame& f) {sinkOutbound.sink(const can::DataFrame& f);}
//   virtual void sinkOutbound_2(const can::DataFrame& f) {sinkOutbound.sink(const can::DataFrame& f);}

//   virtual void sink(const can::DataFrame& f);

private:
   virtual void handle();

   int             m_epoll_fd;
   int             m_fd;
   std::string     m_name;
   can::FrameSink* m_sinkInbound_1;
   can::FrameSink* m_sinkInbound_2;

   size_t read_port(int fd, uint8_t * buffer, size_t size);

   uint32_t HextoDec(unsigned const char *hex, size_t hexlen);

   logging::ostream* m_log;
   std::string       m_log_prefix;
   std::string       m_log_color;
   std::string       m_log_color_reset;
};

}

#endif /* CORE_USBINTERFACE_USBINTERFACE_HPP_ */
