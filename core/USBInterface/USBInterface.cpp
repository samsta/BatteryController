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
    m_sink(nullptr),
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

void USBPort::setSink(can::FrameSink& sink)
{
   m_sink = &sink;
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
	uint bread;
	uint8_t inbuf[100];

	bread = read_port(m_fd, inbuf, 100);
	if (bread > 0)
	{
	  printf("bread= %d:: ", bread);
	  for (uint i=0; i<bread; i++)
	  {
		printf(" %x ", inbuf[i]);
	  }
	  printf("\n");
	}

  //struct can_frame frame;

  uint32_t findhash, port, canid;
  std::string sbuf((char *)inbuf);

  // find #, ID is 8 chars before it
  findhash = sbuf.find_first_of('#');
  if (findhash > 7)
  {
    // msg format 0P000xxx P=port xxx=msg id
    // get the can port number from the long id
    port = HextoDec( &inbuf[findhash - 7], 1);
    canid = HextoDec( &inbuf[findhash - 3], 3);
    if (port > 0 && port <= 2 && canid > 0  && canid <= 0x6FF)
    {
      // construct CAN message from received data

    	//   int nbytes = read(m_fd, &frame, sizeof(frame));
    	//
    	//   if (nbytes < 0) {
    	//      std::cerr << "Error on reading socket " << m_name << ": " << strerror(errno) << std::endl;
    	//      return;
    	//   }
    	//
    	//   if (nbytes < int(sizeof(struct can_frame))) {
    	//      std::cerr << "Read incomplete CAN frame on socket " << m_name << ": " << strerror(errno) << std::endl;
    	//      return;
    	//   }
    	//
    	//   if (m_sink == nullptr)
    	//   {
    	//      std::cerr << "Don't have a sink for socket " << m_name << " so can't receive CAN frames";
    	//      return;
    	//   }
    	//
    	//   m_sink->sink(
    	//      can::StandardDataFrame(frame.can_id, frame.data, frame.can_dlc));




//      CAN_message_t canmsg;
//      canmsg.id = canid;
//      canmsg.seq = 1;
//      canmsg.len = (uBytesRead - 9) / 2;
//      for (uint8_t i=0 ; i < canmsg.len; i++ )
//      {
//        canmsg.buf[i] = HextoDec( &bByteBuffer[i*2 + 9], 2);
//      }

    //  if (port == 1)
    //  {
    //    CANPort1.write(canmsg);
    //  }
    //  else if (port == 2)
    //  {
    //    CANPort2.write(MB15, canmsg);
    //  }
      // diagnostics
      // Serial.print("PORT=");
      // Serial.print(port);
      // Serial.print("  ID=");
      // Serial.print(canmsg.id);
      // Serial.print("  bytesread=");
      // Serial.print(uBytesRead);
      // Serial.print("  msg=");
      // for ( uint8_t i = 9; i < uBytesRead; i++ )
      // {
      //   Serial.write(bByteBuffer[i]);
      // }
      // Serial.println();

    }
    else
    {
      // error, discard data
      std::cerr << "USBPort: ERROR invalid port or canid" << std::endl;
    }
  }
  else
  {
    // error, discard data
    std::cerr << "USBPort: ERROR: bad msg format" << std::endl;
  }

}

void USBPort::sink(const can::DataFrame& f)
{
   if (m_log)
   {
      *m_log << m_log_color << m_log_prefix << f << m_log_color_reset << std::endl;
   }

   struct can_frame frame;
   frame.can_id = f.id();
   frame.can_dlc = f.size();
   std::copy(f.begin(), f.end(), frame.data);

   if (write(m_fd, &frame, sizeof(frame)) != sizeof(frame))
   {
      std::cerr << "Error on writing socket " << m_name << ": " << strerror(errno) << std::endl;
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
