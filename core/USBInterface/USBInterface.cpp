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

USBPort::USBPort(const char* name, int epoll_fd, logging::Logger *log):
    m_unprocessedSize(0),
    m_epoll_fd(epoll_fd),
    m_fd(open_serial_port(name, 9600)),
    m_name(name),
    m_sinkInbound{nullptr, nullptr, nullptr},
    m_packs{
        {m_fd, 0, log},// m_log_prefix, m_log_color, m_log_color_reset},
        {m_fd, 1, log},// m_log_prefix, m_log_color, m_log_color_reset},
        {m_fd, 2, log}//, m_log_prefix, m_log_color, m_log_color_reset}
    },
    m_log_prefix(),
    m_log_color(),
    m_log_color_reset(),
    m_log(log),
    m_class_name(__func__)
{
   struct epoll_event ev;

   ev.events = EPOLLIN;
   ev.data.ptr = this;
   if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_fd, &ev) == -1) {
      std::cerr << "ERROR in epoll_ctl(): Failed adding CanPort " << name << " to epoll: " << strerror(errno) << std::endl;
      std::string smsg;
      smsg.append("USBPort: ERROR in epoll_ctl(): Failed adding CanPort ");
      smsg.append(name);
      smsg.append(" to epoll: ");
      smsg.append(strerror(errno));
      if (m_log) m_log->error(smsg, __FILENAME__,__LINE__);
      exit(EXIT_FAILURE);
   }
   std::string ss;
   ss.append("USBPort Initialized: ");
   ss.append(m_name);
   if (m_log) m_log->info(ss, __FILENAME__, __LINE__);
}

USBPort::~USBPort()
{
   epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_fd, NULL);
}

void USBPort::setSinkInbound(unsigned index, can::FrameSink& sink)
{
   // Check it's within range. I'll just cap it a NUM_PACKS - 1
   // but maybe there's something else you want to do?
   // todo report an error?
   if (index >= NUM_PACKS) index = NUM_PACKS - 1;
   m_sinkInbound[index] = &sink;
}

void USBPort::setupLogger( const char* logger_prefix, const char* logger_color)
{
   m_log_prefix = logger_prefix;
   if (logger_color)
   {
      m_log_color  = logger_color;
      m_log_color_reset = color::reset;
   }
}

void USBPort::handle()
{
   uint bytesread;
   size_t findhash,findinfo;
   uint16_t port;
   uint16_t canid;
   struct can_frame frame;
   char cbuf[2048];

   // add the new data to the leftover data from the last read
   bytesread = read_port(m_fd, &m_inBufferUnprocessed[m_unprocessedSize], sizeof(m_inBufferUnprocessed)- m_unprocessedSize - 1);
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
               sprintf(cbuf, "TEENSY:  %.*s", (int)findhash-1, m_inBufferUnprocessed);
            }
            else
            {
               sprintf(cbuf,"TEENSY:  MESSAGE OVERSIZE, CAN'T BE DISPLAYED");
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
         // find #, ID is 8 chars before it
         findhash = sbuf.find_first_of('#');
         if (findhash == 8 && m_unprocessedSize >= STD_MSG_SIZE)
         {
            // msg format 0P000xxx P=port xxx=msg id
            // get the can port number from the long id
            port = HextoDec( &m_inBufferUnprocessed[findhash - 7], 1);
            canid = HextoDec( &m_inBufferUnprocessed[findhash - 3], 3);

            if (port > 0 && port <= 2 && canid > 0  && canid <= 0x7FF)
            {
               // construct CAN message from received data
               frame.can_id = canid;
               //canmsg.seq = 1;
               frame.can_dlc = 8;
               int z = frame.can_dlc;
               z++;
               for (uint8_t i=0 ; i < 8; i++ )
               {
                  frame.data[i] = HextoDec( &m_inBufferUnprocessed[i*2 + 9], 2);
               }

               // printf("Received from port = %d\n", port);//JFS
               // use the port number to know where to send it
               if (m_sinkInbound[port-1] != nullptr) {
                  m_sinkInbound[port-1]->sink(can::StandardDataFrame(frame.can_id, frame.data, frame.can_dlc));
               }
               else {
                  sprintf(cbuf, "USBPORT: Unexpected CAN msg received on Teensy port %d", port);
                  if (m_log) m_log->error(cbuf,__FILENAME__,__LINE__);
               }
            }
            else
            {
               // error, discard data
               if (!(port > 0 && port <= 2))
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
         else if (findhash != 8)
         {
            // error, discard data
            // display bad msg 
            if (findhash < (sizeof(cbuf)-100))
            {
               sprintf(cbuf, "USBPort: Receive ERROR: bad msg format: fh= %d  br= %d  %.*s", (int)findhash, m_unprocessedSize, m_unprocessedSize, m_inBufferUnprocessed);
            }
            else
            {
               sprintf(cbuf,"USBPort: Receive ERROR: bad msg format: MESSAGE OVERSIZE, CAN'T BE DISPLAYED");
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

USBPort::Pack::Pack(int fd,
      unsigned index,
      logging::Logger* log
      // ,
      // std::string log_prefix,
      // std::string log_color,
      // std::string log_color_reset
      ):
   m_fd(fd), 
   m_index(index),
   m_log(log)
   // ,
   // m_log_prefix(log_prefix),
   // m_log_color(log_color),
   // m_log_color_reset(log_color_reset)
{}

void USBPort::Pack::sink(const can::DataFrame& f)
{
  if (m_log)
  {
      std::ostringstream ss;
      ss << "<USB OUT port " << m_index << ">" << f;
      if (m_log) m_log->debug(ss);
  }
   char msg[100];
   uint8_t uint8msg[26];

   // destination port
   sprintf(&msg[0],"%02x00", m_index+1);

   // canid
   sprintf(&msg[4],"0%3x#", f.id());

   // 16 hex bytes for can data (8 bytes)
   for (int i=0; i<(int)f.size(); i++ )
   {
      sprintf(&msg[9+(i*2)], "%02x", f.data()[i]);
   }
   // there are 25 characters in the message 8+1+16
   for (int i=0; i<25; i++)
   {
      uint8msg[i] = (uint8_t) msg[i];
   }
   // put a CR at the end of the message
   uint8msg[25] = 0xd;

   // printf("Sending to port = %d\n",m_index+1);//JFS
   int x =  write(m_fd, uint8msg, sizeof(uint8msg));
   if (x<0)
   {
      std::ostringstream ss;
      ss << "WRITE TO USB PORT FAILED: Port " << m_index;
      if (m_log) m_log->error(ss, __FILENAME__,__LINE__);
      // std::cerr << "WRITE TO USB PORT FAILED" << std::endl;
      // fflush(stdout);
   }
}

// Opens the specified serial port, sets it up for binary communication,
// configures its read timeouts, and sets its baud rate.
// Returns a non-negative file descriptor on success, or -1 on failure.
int USBPort::open_serial_port(const char * device, uint32_t baud_rate)
{
  int fd = open(device, O_RDWR | O_NOCTTY);
  if (fd == -1)
  {
     std::cerr << "usb port: failed to open port: " << device << std::endl;
    return -1;
  }

  // Flush away any bytes previously read or written.
  int result = tcflush(fd, TCIOFLUSH);
  if (result)
  {
     std::cerr << "usb port: warning tcflush failed" << std::endl;  // just a warning, not a fatal error
  }

  // Get the current configuration of the serial port.
  struct termios options;
  result = tcgetattr(fd, &options);
  if (result)
  {
     std::cerr << "us port: tcgetattr failed" << std::endl;
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
  options.c_cc[VTIME] = 1;
  options.c_cc[VMIN] = 0;

  // This code only supports certain standard baud rates. Supporting
  // non-standard baud rates should be possible but takes more work.
  switch (baud_rate)
  {
  case 4800:   cfsetospeed(&options, B4800);   break;
  case 9600:   cfsetospeed(&options, B9600);   break;
  case 19200:  cfsetospeed(&options, B19200);  break;
  case 38400:  cfsetospeed(&options, B38400);  break;
  case 115200: cfsetospeed(&options, B115200); break;
  default:
      std::cerr << "usb port warning: baud rate " << baud_rate << "is not supported, using 9600." << std::endl;
    cfsetospeed(&options, B9600);
    break;
  }
  cfsetispeed(&options, cfgetospeed(&options));

  result = tcsetattr(fd, TCSANOW, &options);
  if (result)
  {
    std::cerr << "usb port: tcsetattr failed" << std::endl;
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
