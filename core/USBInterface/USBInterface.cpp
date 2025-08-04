/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "USBInterface.hpp"
#include "can/StandardDataFrame.hpp"
#include "logging/colors.hpp"
#include <errno.h>
#include <iostream>
#include <linux/can/raw.h>
#include <linux/can.h>
#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fcntl.h>
#include <stdint.h>
#include <termios.h>

namespace color = logging::color::ansi;

namespace core {


// CLEAN UP USAGE OF M_NAME AND M_USB_ID

// FIGURE OUT HOW TO ASSIGN A PACK NAME TO M_PACKS


USBPort::USBPort(const char* port_name, int epoll_fd, logging::Logger *log):
    m_unprocessedSize(0),
    m_epoll_fd(epoll_fd),
    m_fd(open_serial_port(port_name, log)),
    m_port_name(port_name),
    m_sinkInbound{nullptr, nullptr, nullptr},
    m_packs{
        {m_fd, 0, port_name, log},
        {m_fd, 1, port_name, log},
        {m_fd, 2, port_name, log}
    },
    m_log(log),
    m_class_name(__func__)
{
   struct epoll_event ev;

   ev.events = EPOLLIN;
   ev.data.ptr = this;
   if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_fd, &ev) == -1) {
      std::cerr << "ERROR in epoll_ctl(): Failed adding USBPort " << port_name << " to epoll: " << strerror(errno) << std::endl;
      std::string smsg;
      smsg.append("USBPort:" + m_port_name +": ");
      smsg.append("ERROR in epoll_ctl(): Failed adding USBPort ");
      smsg.append(m_port_name);
      smsg.append(" to epoll: ");
      smsg.append(strerror(errno));
      if (m_log) m_log->error(smsg, __FILENAME__,__LINE__);
      exit(EXIT_FAILURE);
   }
   std::string ss;
   ss.append("USBPort: " + m_port_name +": ");
   ss.append("Initialized.");
   if (m_log) m_log->info(ss, __FILENAME__, __LINE__);
}

USBPort::~USBPort()
{
   epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_fd, NULL);
}

void USBPort::setSinkInbound(unsigned index, char* pack_name, can::FrameSink& sink)
{
   // Check it's within range. I'll just cap it a NUM_PACKS - 1
   // but maybe there's something else you want to do?
   // todo report an error?
   if (index >= NUM_PACKS) index = NUM_PACKS - 1;
   m_sinkInbound[index] = &sink;
   m_packs[index].setPackName(pack_name);
}

void USBPort::handle()
{
   uint bytesread;
   size_t findhash,findinfo;
   uint16_t port;
   uint32_t canid;
   struct can_frame frame;
   char cbuf[2048];

   // add the new data to the leftover data from the last read
   bytesread = read_port(m_fd, &m_inBufferUnprocessed[m_unprocessedSize], sizeof(m_inBufferUnprocessed)- m_unprocessedSize - 1);
   // check for error
   if (bytesread < 0) 
   {
      m_log->error("Error reading from serial port.",__FILENAME__,__LINE__);
   }
   m_unprocessedSize += bytesread;

   uint32_t newhead = 0;
   uint32_t loopcount = 0;
   while (m_unprocessedSize >= STD_MSG_SIZE and loopcount < 100)
   {
      loopcount++;
      std::string sbuf((char *)m_inBufferUnprocessed);
      // find DIAGTEST
      findhash = sbuf.find("DIAG");
      if ((findhash != std::string::npos) && (findhash < STD_MSG_SIZE))
      {
         bool is_info = false;
         // is this INFO or ERROR?
         findinfo = sbuf.find_first_of("INFO");
         if (findinfo != std::string::npos && findinfo < 10)
         {
            is_info = true;
         } 
         findhash = sbuf.find_first_of(0x0a);
         if (findhash != std::string::npos and findhash < sizeof(m_inBufferUnprocessed)) {
            if (findhash < (sizeof(cbuf)-100))
            {
               sprintf(cbuf, "%s: TEENSY:  %.*s", m_port_name.c_str(), (int)findhash-1, m_inBufferUnprocessed);
            }
            else
            {
               sprintf(cbuf,"%s: TEENSY: MESSAGE OVERSIZE, CAN'T BE DISPLAYED",m_port_name.c_str());
            }
            newhead = findhash + 1;
            m_unprocessedSize = m_unprocessedSize - newhead;
         }
         else
         {
            // printf("Failed to find 0x0a at end of Diagnostic Msg\n");
            // fflush(stdout);
            sprintf(cbuf, "TEENSY: Failed to find 0x0a at end of Diagnostic Msg");
            // is_info = false;
            m_unprocessedSize = 0;
         }
         if (m_log) {
            if (is_info) m_log->info(cbuf,__FILENAME__,__LINE__);
            else m_log->error(cbuf,__FILENAME__,__LINE__);
         }
      }
      else
      {
         // this better be a hex encoded CAN message
         // find # (hash)
         findhash = sbuf.find_first_of('#');
         if (findhash == HASH_OFFSET && m_unprocessedSize >= STD_MSG_SIZE)
         {
            // msg format 0Pxxxxxxxx#yyyyyyyyyyyyyyyy P=port xxxxxxxx=can id (8 bytes) hex y..=can msg hex (8 bytes)
            // get the can port number from the long id
            port = HextoDec( &m_inBufferUnprocessed[PORT_OFFSET], 1);
            canid = HextoDec( &m_inBufferUnprocessed[CAN_OFFSET], CAN_LEN);

            if (port > 0 && port <= NUM_PACKS && canid > 0  && canid <= 0x1FFFFFFF)
            {
               // construct CAN message from received data
               frame.can_id = canid;
               frame.can_dlc = DATA_LEN;
               for (uint8_t i=0 ; i < DATA_LEN; i++ )
               {
                  frame.data[i] = HextoDec( &m_inBufferUnprocessed[i*2 + DATA_OFFSET], 2);
               }

               can::StandardDataFrame canframe(frame.can_id, frame.data, frame.can_dlc);
               if (m_log && m_log->isdebug()) {
                  std::ostringstream ss;
                  ss << "<USB IN:" << m_port_name << " CAN port:" << port << " Id:" << canid << "> " << canframe;
                  m_log->debug(ss);
               }
               // use the port number to know where to send it
               if (m_sinkInbound[port-1] != nullptr) {
                  m_sinkInbound[port-1]->sink(can::StandardDataFrame(canframe));
               }
               else {
                  sprintf(cbuf, "%s: Unexpected CAN msg received on Teensy port %d", m_port_name.c_str(), port);
                  if (m_log) m_log->alarm(cbuf,__FILENAME__,__LINE__);
               }
            }
            else
            {
               // error, discard data
               if (!(port > 0 && port <= NUM_PACKS))
               {
                  // todo port error
               }
               if (!(canid > 0  && canid <= 0x7FF))
               {
                  // todo canid error
               }
            }
            newhead = STD_MSG_SIZE;
            m_unprocessedSize = m_unprocessedSize - newhead;
         }
         else if (findhash != HASH_OFFSET)
         {
            // error, discard data
            // display bad msg 
            if (findhash < (sizeof(cbuf)-100))
            {
               sprintf(cbuf, "%s: Receive ERROR: bad msg format: fh= %d  br= %d  %.*s", m_port_name.c_str(), (int)findhash, m_unprocessedSize, m_unprocessedSize, m_inBufferUnprocessed);
            }
            else
            {
               sprintf(cbuf,"%s: Receive ERROR: bad msg format: MESSAGE OVERSIZE, CAN'T BE DISPLAYED", m_port_name.c_str());
            }

            if (m_log) m_log->error(cbuf, __FILENAME__,__LINE__);
            m_unprocessedSize = 0;

         }
      }
      // adjust the data in inbuf
      if (m_unprocessedSize != 0)
      {
         memcpy(&m_inBufferUnprocessed[0], &m_inBufferUnprocessed[newhead], m_unprocessedSize);
      }
   } // while (m_unprocessedSize >= STD_MSG_SIZE and loopcount < 100)
}

can::FrameSink& USBPort::getSinkOutbound(unsigned index)
{
   // Check it's within range. I'll just cap it a NUM_PACKS - 1
   // but maybe there's something else you want to do?
   // todo report an error?
   if (index >= NUM_PACKS) index = NUM_PACKS - 1;
   return m_packs[index];
}

USBPort::Pack::Pack(int fd, unsigned index, const char* usbport_name, logging::Logger* log):
   m_fd(fd), 
   m_index(index),
   m_usbport_name(usbport_name),
   m_pack_name(usbport_name),
   m_log(log)
{}

void USBPort::Pack::setPackName(const char* pack_name)
{
   m_pack_name = pack_name;
}
void USBPort::Pack::sink(const can::DataFrame& f)
{
   char msg[100];
   uint8_t uint8msg[STDmsgsize + 1]; // +1 for CR at the end

   // destination port
   sprintf(&msg[PORToffset],"%02x", m_index+1);

   // canid
   sprintf(&msg[CANoffset],"%08x#", f.id());

   // 16 hex bytes for can data (8 bytes)
   for (int i=0; i<(int)f.size(); i++ )
   {
      sprintf(&msg[DATAoffset+(i*2)], "%02x", f.data()[i]);
   }
   // there are 27 characters in the message 2+8+1+16
   for (int i=0; i<STDmsgsize; i++)
   {
      uint8msg[i] = (uint8_t) msg[i];
   }
   // put a CR at the end of the message
   uint8msg[STDmsgsize] = 0xd;

   if (m_log && m_log->isdebug())
   {
      std::ostringstream ss;
      ss << "<USB OUT:" << m_usbport_name << " " << m_pack_name << " CAN port:" << (m_index+1) << "> " << f;
      m_log->debug(ss);
      std::ostringstream xx;
      // xx << "<USB OUT:hex:";
      // for (size_t i = 0; i < sizeof(uint8msg); i++) {
      //    xx << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(uint8msg[i]) << " ";
      // }
      // m_log->debug(xx);
      std::ostringstream oss;
      oss << "<USB OUT:ascii:";
      for (size_t i = 0; i < (sizeof(uint8msg)-1); ++i) {
         if (std::isprint(uint8msg[i])) {
            oss << static_cast<char>(uint8msg[i]);
         } else {
            oss << '.';  // unprintable character
         }
      }
      m_log->debug(oss);
   }

   int x =  write(m_fd, uint8msg, sizeof(uint8msg));
   if (x<0)
   {
      std::ostringstream ss;
      ss << "WRITE TO USB PORT FAILED:" << m_usbport_name << " (" << m_pack_name << ")";
      if (m_log) m_log->error(ss, __FILENAME__,__LINE__);
   }
}

// Opens the specified serial port, sets it up for binary communication,
// configures its read timeouts, and sets its baud rate.
// Returns a non-negative file descriptor on success, or -1 on failure.
int USBPort::open_serial_port(const char * device, logging::Logger* log)
{
  int fd = open(device, O_RDWR | O_NOCTTY);
  if (fd == -1)
  {
      std::cerr << "usb port: failed to open port: " << device << std::endl;
      std::ostringstream ss;
      ss << "usb port: failed to open port: " << device << " error: " << strerror(errno);
      if (log) log->error(ss,__FILENAME__,__LINE__);
      return -1;
  }

  // Flush away any bytes previously read or written.
  int result = tcflush(fd, TCIOFLUSH);
  if (result)
  {
     std::cerr << "usb port: warning tcflush failed" << std::endl;  // just a warning, not a fatal error
      if (log) log->alarm("usb port: warning tcflush failed",__FILENAME__,__LINE__);
  }

  // Get the current configuration of the serial port.
  struct termios options;
  result = tcgetattr(fd, &options);
  if (result)
  {
      std::cerr << "usb port: tcgetattr failed" << std::endl;
      if (log) m_log->error("usb port: tcgetattr failed",__FILENAME__,__LINE__);
      close(fd);
      return -1;
  }

  // Turn off any options that might interfere with our ability to send and
  // receive raw binary bytes.
  options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
  options.c_oflag &= ~(ONLCR | OCRNL);
  options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);

  // Set up timeouts: Calls to read() will return as soon as there is
  // at least one byte available or when 100 ms has passed.
  // THE ABOVE IS ONLY TRUE, if buffer size is 1 byte,
  // when bytes start to flow into the buffer, the read doesn't return
  // until the buffer is full
  // for a NON-BLOCKING READ....see below  
  // https://www.i-programmer.info/programming/cc/10027-serial-c-and-the-raspberry-pi.html?start=4
  // at some point it may be good to change this to a non-block read, good info on this in the above link
  options.c_cc[VTIME] = 1;
  options.c_cc[VMIN] = 0;

  // when opening USB ports the baud rate setting is ignored,
  // it gets set to the USB port rate...1 MB.. or whatever it is.
  // but you need to set it, apparently
   cfsetospeed(&options, B9600);

  result = tcsetattr(fd, TCSANOW, &options);
  if (result)
  {
      std::cerr << "usb port: tcsetattr failed" << std::endl;
      if (log) log->error("usb port: tcsetattr failed",__FILENAME__,__LINE__);
      close(fd);
      return -1;
  }

  return fd;
}

// Reads bytes from the serial port.
// Returns after all the desired bytes have been read, or if there is a
// timeout or other error.
// Returns the number of bytes successfully read into the buffer, or -1 if
// there was an error reading.
size_t USBPort::read_port(int fd, uint8_t * buffer, size_t size)
{
   size_t received = 0;
   while (received < size)
   {
      ssize_t r = read(fd, buffer + received, size - received);
      if (r < 0)
      {
         perror("failed to read from port");
         return -1;
      }
      if (r == 0)
      {
         // Timeout
         break;
      }
      received += r;
   }
   return received;
}

static const long hextable[] = {
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
   -1,-1, 0,1,2,3,4,5,6,7,8,9,-1,-1,-1,-1,-1,-1,-1,10,11,12,13,14,15,-1,
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
   -1,-1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
   -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
};

uint32_t USBPort::HextoDec(unsigned const char *hex, size_t hexlen) {
   uint32_t dec = 0;
   for (size_t i=0; i<hexlen; i++)
   {
      dec = (dec << 4) | hextable[*hex++];
   }
   return dec;
}
} // namespace core
