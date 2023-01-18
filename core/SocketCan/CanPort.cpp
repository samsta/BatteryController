/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "CanPort.hpp"
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

namespace color = logging::color::ansi;

namespace core {
namespace {

int openSocket(const char* name)
{
   struct sockaddr_can addr;
   struct ifreq ifr;
   int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

   strcpy(ifr.ifr_name, name);
   if (ioctl(s, SIOCGIFINDEX, &ifr) != 0)
   {
      std::cerr << "Error ioctl'ing on socket " << name << ":" << strerror(errno) << std::endl;
      exit(EXIT_FAILURE);
   }
   addr.can_family = AF_CAN;
   addr.can_ifindex = ifr.ifr_ifindex;

   if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) != 0)
   {
      std::cerr << "Error binding to socket " << name << ":" << strerror(errno) << std::endl;
      exit(EXIT_FAILURE);
   }
   return s;
}

}


CanPort::CanPort(const char* name, int epoll_fd):
    m_epoll_fd(epoll_fd),
    m_fd(openSocket(name)),
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

CanPort::~CanPort()
{
   epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_fd, NULL);
}

void CanPort::setSink(can::FrameSink& sink)
{
   m_sink = &sink;
}

void CanPort::setupLogger(
      CPlusPlusLogging::Logger& log,
      const char* logger_prefix,
      const char* logger_color)
{
   m_log = &log;
   m_log_prefix = logger_prefix;
   if (logger_color)
   {
      m_log_color  = logger_color;
      m_log_color_reset = color::reset;
   }
}

void CanPort::handle()
{
   struct can_frame frame;
   int nbytes = read(m_fd, &frame, sizeof(frame));

   if (nbytes < 0) {
      std::cerr << "Error on reading socket " << m_name << ": " << strerror(errno) << std::endl;
      return;
   }

   if (nbytes < int(sizeof(struct can_frame))) {
      std::cerr << "Read incomplete CAN frame on socket " << m_name << ": " << strerror(errno) << std::endl;
      return;
   }

   if (m_sink == nullptr)
   {
      std::cerr << "Don't have a sink for socket " << m_name << " so can't receive CAN frames";
      return;
   }

   m_sink->sink(
      can::StandardDataFrame(frame.can_id, frame.data, frame.can_dlc));
}

void CanPort::sink(const can::DataFrame& f)
{
   std::ostringstream ss;
   if (m_log)
   {
      ss << m_log_color << m_log_prefix << f << m_log_color_reset;
      m_log->debug(ss);
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


}
