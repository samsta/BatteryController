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
namespace {

// Opens the specified serial port, sets it up for binary communication,
// configures its read timeouts, and sets its baud rate.
// Returns a non-negative file descriptor on success, or -1 on failure.
int open_serial_port(const char * device, uint32_t baud_rate)
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

} // namespace

USBPort::USBPort(const char* name, int epoll_fd):
    m_epoll_fd(epoll_fd),
    m_fd(open_serial_port(name, 9600)),
    m_name(name),
    m_sink_1(nullptr),
    m_sink_2(nullptr),
    m_log(nullptr),
    m_log_prefix(),
    m_log_color(),
    m_log_color_reset()
{
   struct epoll_event ev;

   ev.events = EPOLLIN;
   ev.data.ptr = this;
   if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_fd, &ev) == -1) {
      std::cerr << "ERROR in epoll_ctl(): Failed adding CanPort " << name << " to epoll: " << strerror(errno) << std::endl;
      exit(EXIT_FAILURE);
   }
}

USBPort::~USBPort()
{
   epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_fd, NULL);
}

void USBPort::setSink_1(can::FrameSink& sink)
{
   m_sink_1 = &sink;
}

void USBPort::setSink_2(can::FrameSink& sink)
{
   m_sink_2 = &sink;
}

void USBPort::setupLogger( logging::ostream& log, const char* logger_prefix, const char* logger_color)
{
   m_log = &log;
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
   uint8_t inbuf[1000];
   size_t findhash;
   uint16_t port;
   uint16_t canid;
   struct can_frame frame;

   // must clear out inbuf everytime, perhaps there's a better way to do this?
   // this is really only necessary because we are printf-ing the contents below
   for (uint32_t i=0; i<sizeof(inbuf); i++)
   {
    inbuf[i] = 0;
   }
   bytesread = read_port(m_fd, inbuf, sizeof(inbuf));

   uint32_t newhead = 0;
   uint32_t loopcount = 0;
   while (bytesread > 0 and loopcount < 10) {
      loopcount++;
      printf("HANDLE br= %d  %.*s\n",bytesread, bytesread, inbuf);
      fflush(stdout);

      std::string sbuf((char *)inbuf);
      // find DIAGTEST
      findhash = sbuf.find("DIAG");
      if (findhash != std::string::npos)
      {
         printf("TEENSY Diagnostic Message: ");
         findhash = sbuf.find_first_of(0x0a);
         if (findhash != std::string::npos and findhash < sizeof(inbuf)) {
            printf("%.*s\n", (int)findhash, inbuf);
            fflush(stdout);
            newhead = findhash + 1;
            bytesread = bytesread - newhead;
         }
         else
         {
            printf("Failed to find 0x0a End of Msg\n");
            bytesread = 0;
         }
      }
      else
      {
         // this better be a hex encoded CAN message
         // find #, ID is 8 chars before it
         findhash = sbuf.find_first_of('#');
         if (findhash == 8 && bytesread >= 25)
         {
            // msg format 0P000xxx P=port xxx=msg id
            // get the can port number from the long id
            port = HextoDec( &inbuf[findhash - 7], 1);
            canid = HextoDec( &inbuf[findhash - 3], 3) - 0x600;

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
                  frame.data[i] = HextoDec( &inbuf[i*2 + 9], 2);
               }

               // todo something has to be done with the port number to know where to send it
               if (port == 1) {
                  m_sink_1->sink(can::StandardDataFrame(frame.can_id, frame.data, frame.can_dlc));
               }
               else {
                  m_sink_2->sink(can::StandardDataFrame(frame.can_id, frame.data, frame.can_dlc));
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
            newhead = 25;
            bytesread = bytesread - newhead;
         }
         else
         {
            // error, discard data
            std::cerr << "USBPort: ERROR: bad msg format" << std::endl;

            // display bad msg (or diagnostic message)
            printf("br= %d  %s\n",bytesread, inbuf);
            bytesread = 0;

         }
      }

      // adjust the data in inbuf
      if (bytesread != 0)
      {
         memcpy(&inbuf[0], &inbuf[newhead], bytesread);
      }
   }
}

void USBPort::sink(const can::DataFrame& f)
{


   if (m_log)
   {
      *m_log << m_log_color << m_log_prefix << f << m_log_color_reset << std::endl;
   }
   char msg[100];
   uint8_t uint8msg[25];

   // todo destination port
   int packnumber = 1;
   sprintf(&msg[0],"%02x00", packnumber);

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

//   printf("SENDING: %s\n", msg);
   int x =  write(m_fd, uint8msg, sizeof(uint8msg));
   if (x<0)
   {
    printf("WRITE FAILED");
     fflush(stdout);
   }
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
