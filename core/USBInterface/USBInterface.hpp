/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef CORE_USBINTERFACE_USBINTERFACE_HPP_
#define CORE_USBINTERFACE_USBINTERFACE_HPP_

#include "can/FrameSink.hpp"
#include "core/Linux/EpollHandler.hpp"
#include "logging/stream.hpp"
#include <string>

namespace core
{

class USBPort: private core::EpollHandler
{
public:
   USBPort(const char* name, int epoll_fd);
   ~USBPort();

   void setSinkInbound(unsigned index, can::FrameSink& sink);

   can::FrameSink& getSinkOutbound(unsigned index);

   void setupLogger(
         logging::ostream& log,
         const char* logger_prefix = "",
         const char* logger_color = nullptr);

   static const unsigned NUM_PACKS = 3;
   
private:
   virtual void handle();

   int             m_epoll_fd;
   int             m_fd;
   std::string     m_name;
   can::FrameSink* m_sinkInbound[NUM_PACKS];

   class Pack: public can::FrameSink
   {
   public:
      Pack(int fd,
         unsigned index,
         logging::ostream* m_log
         // ,
         // std::string m_log_prefix,
         // std::string m_log_color,
         // std::string m_log_color_reset
         );
   private:
      int m_fd;
      unsigned m_index;
      logging::ostream* m_log;
      // std::string       m_log_prefix;
      // std::string       m_log_color;
      // std::string       m_log_color_reset;

      virtual void sink(const can::DataFrame& f);
   };

   Pack m_packs[NUM_PACKS];

   int open_serial_port(const char * device, uint32_t baud_rate);
   size_t read_port(int fd, uint8_t * buffer, size_t size);

   uint32_t HextoDec(unsigned const char *hex, size_t hexlen);

   logging::ostream* m_log;
   std::string       m_log_prefix;
   std::string       m_log_color;
   std::string       m_log_color_reset;
};

// //---------------------------------------------------------------------------------------------------
// class TeensyRelay
// {
// public:
//    TeensyRelay(can::FrameSink& sender, uint32_t canid);
//    // ~TeensyRelay();

//    bool getRelayState();
//    void setRelayState(bool newstate);

// private:
//    can::FrameSink&   m_sender;
//    uint32_t m_canid;
//    bool m_relay_state;

//    const uint8_t m_off_msg[8] = { 0x55, 0x55, 0x00, 0x00, 0x00, 0x00 };
//    const uint8_t m_on_msg[8] = { 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00 };
// };

// //---------------------------------------------------------------------------------------------------
// class PackSafetyLBC
// {
// public:
//    PackSafetyLBC(can::FrameSink& sender);
//    PackSafetyLBC();
//    // ~PackSafetyLBC();

//    void setSafetyRelayState(bool newstate);
//    void setLBCRelayState(bool newstate);
//    bool getSafetyRelayState();
//    bool getLBCRelayState();

// private:
//    TeensyRelay m_safetyrelay;
//    TeensyRelay m_LBCrelay;
// };

}
#endif /* CORE_USBINTERFACE_USBINTERFACE_HPP_ */
