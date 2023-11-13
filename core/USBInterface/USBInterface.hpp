/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CORE_USBINTERFACE_USBINTERFACE_HPP_
#define CORE_USBINTERFACE_USBINTERFACE_HPP_

#include "can/FrameSink.hpp"
#include "core/Linux/EpollHandler.hpp"
#include "logging/stream.hpp"
#include <logging/logging.hpp>
#include <string>

namespace core
{

class USBPort: private core::EpollHandler
{
public:
   USBPort(const char* name, int epoll_fd, logging::Logger* log);
   ~USBPort();

   void setSinkInbound(unsigned index, char* pack_name, can::FrameSink& sink);
   can::FrameSink& getSinkOutbound(unsigned index);

   static const unsigned NUM_PACKS = 3;
   
private:
   virtual void handle();

   uint8_t m_inBufferUnprocessed[1024];
   uint m_unprocessedSize;
   const uint STD_MSG_SIZE = 25;

   int             m_epoll_fd;
   int             m_fd;
   std::string     m_port_name;
   can::FrameSink* m_sinkInbound[NUM_PACKS];

   class Pack: public can::FrameSink
   {
   public:
      Pack(int fd, unsigned index, const char* usbport_name, logging::Logger* m_log);
      void setPackName(const char* pack_name);
   private:
      int m_fd;
      const char* m_pack_name;
      const char* m_usbport_name;
      unsigned m_index;
      logging::Logger* m_log;
      virtual void sink(const can::DataFrame& f);
   };

   Pack m_packs[NUM_PACKS];

   int open_serial_port(const char * device, logging::Logger* log);
   size_t read_port(int fd, uint8_t * buffer, size_t size);

   uint32_t HextoDec(unsigned const char *hex, size_t hexlen);

   logging::Logger* m_log;

   std::string m_class_name;
};

}

#endif /* CORE_USBINTERFACE_USBINTERFACE_HPP_ */
