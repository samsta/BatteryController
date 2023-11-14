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


CanPort::CanPort(const char* name, int epoll_fd, logging::Logger *log):
    m_epoll_fd(epoll_fd),
    m_fd(openSocket(name)),
    m_name(name),
    m_sink(nullptr),
    m_log(log)
{
   struct epoll_event ev;

   ev.events = EPOLLIN;
   ev.data.ptr = this;
   if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_fd, &ev) == -1) {
      std::cerr << "ERROR in epoll_ctl(): Failed adding CanPort " << name << " to epoll: " << strerror(errno) << std::endl;
      std::string smsg;
      smsg.append("CANPort:" + m_name +": ");
      smsg.append("ERROR in epoll_ctl(): Failed adding CanPort ");
      smsg.append(m_name);
      smsg.append(" to epoll: ");
      smsg.append(strerror(errno));
      if (m_log) m_log->error(smsg, __FILENAME__,__LINE__);
      exit(EXIT_FAILURE);
   }
   std::string ss;
   ss.append("CANPort: " + m_name +": ");
   ss.append("Initialized.");
   if (m_log) m_log->info(ss, __FILENAME__, __LINE__);
}

CanPort::~CanPort()
{
   epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, m_fd, NULL);
}

void CanPort::setSink(can::FrameSink& sink)
{
   m_sink = &sink;
}

void CanPort::handle()
{
   struct can_frame frame;
   int nbytes = read(m_fd, &frame, sizeof(frame));

   if (nbytes < 0) {
      std::cerr << "Error on reading socket " << m_name << ": " << strerror(errno) << std::endl;
      std::ostringstream ss;
      ss << "Error on reading socket " << m_name << ": " << strerror(errno);
      if (m_log) m_log->error(ss,__FILENAME__,__LINE__);
      return;
   }

   if (nbytes < int(sizeof(struct can_frame))) {
      std::cerr << "Read incomplete CAN frame on socket " << m_name << ": " << strerror(errno) << std::endl;
      std::ostringstream ss;
      ss << "Read incomplete CAN frame on socket " << m_name << ": " << strerror(errno);
      if (m_log) m_log->error(ss,__FILENAME__,__LINE__);
      return;
   }

   if (m_sink == nullptr)
   {
      std::cerr << "Don't have a sink for socket " << m_name << " so can't receive CAN frames" << std::endl;
      std::ostringstream ss;
      ss << "Don't have a sink for socket " << m_name << " so can't receive CAN frames";
      if (m_log) m_log->error(ss,__FILENAME__,__LINE__);
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
      ss << "<CAN OUT:" << m_name << "> " << f;
      m_log->debug(ss);
   }

   struct can_frame frame;
   frame.can_id = f.id();
   frame.can_dlc = f.size();
   std::copy(f.begin(), f.end(), frame.data);

   if (write(m_fd, &frame, sizeof(frame)) != sizeof(frame))
   {
      std::cerr << "Error on writing socket " << m_name << ": " << strerror(errno) << std::endl;
      std::ostringstream ss;
      ss << "Error on writing socket " << m_name << ": " << strerror(errno);
      if (m_log) m_log->error(ss,__FILENAME__,__LINE__);
   }
}
}
