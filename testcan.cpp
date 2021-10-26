/* SPDX-License-Identifier: GPL-3.0-or-later */

/*
 * This file is prototyping code - don't judge.
 */

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <algorithm>
#include <gpiod.h>
#include <errno.h>

#include "can/StandardDataFrame.hpp"
#include "can/services/Nissan/FrameAggregator.hpp"
#include "can/services/Nissan/GroupPoller.hpp"
#include "can/services/Nissan/MessageFactory.hpp"
#include "can/services/SMA/MessageFactory.hpp"
#include "can/messages/Tesla/DetailedCellData.hpp"
#include "monitor/Nissan/Monitor.hpp"
#include "inverter/SMA/SunnyBoyStorage.hpp"
#include "can/messages/TSUN/InverterChargeDischargeCmd.hpp"
#include "can/messages/TSUN/InverterInfoRequest.hpp"
#include "can/messages/TSUN/InverterSleepAwakeCmd.hpp"
#include "inverter/TSUN/TSOL-H50K.hpp"

#include "contactor/Nissan/LeafContactor.hpp"
#include "core/LibGpiod/OutputPin.hpp"
#include "core/Linux/EpollTimer.hpp"
#include "core/Timer.hpp"
#include "core/Callback.hpp"
#include "logging/colors.hpp"

using namespace core::libgpiod;
namespace color = logging::color::ansi;

class TsunSink: public can::FrameSink
{
public:
   TsunSink(inverter::TSUN::TSOL_H50K& inverter): m_inverter(inverter)
   {
   }

   void* mem()
   {
      return m_message_memory;
   }

   const can::messages::Message* decode(const can::DataFrame& f)
   {
      using namespace can::messages::TSUN;
      can::messages::Message* msg;

      msg = new(mem()) InverterInfoRequest(f);
      if (msg->valid()) return msg;

      msg = new(mem()) InverterChargeDischargeCmd(f);
      if (msg->valid()) return msg;

      msg = new(mem()) InverterSleepAwakeCmd(f);
      if (msg->valid()) return msg;

      return NULL;
   }

   virtual void sink(const can::DataFrame& f)
   {
      const can::messages::Message* msg = decode(f);

      if (msg)
      {
         std::cout << color::bright_green << "<INV IN>  " << *msg << color::reset << std::endl;
         m_inverter.sink(*msg);
      }
   }

   inverter::TSUN::TSOL_H50K& m_inverter;
   uint8_t m_message_memory[1024];
};

class CanSender: public can::FrameSink
{
public:
   CanSender(int fd, const char* name, const char* color): m_fd(fd), m_name(name), m_color(color)
   {
   }

   virtual void sink(const can::DataFrame& f)
   {
      std::cout << m_color << "<" << m_name << " OUT> " << f << color::reset << std::endl;

      struct can_frame frame;
      frame.can_id = f.id();
      frame.can_dlc = f.size();
      std::copy(f.begin(), f.end(), frame.data);

      if (write(m_fd, &frame, sizeof(frame)) != sizeof(frame))
      {
         perror("Writing CAN frame");
      }
   }

private:
   int m_fd;
   const char* m_name;
   const char* m_color;
};

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

int main(int argc, const char** argv)
{
   int battery_socket, inverter_socket;

   if (argc != 3)
   {
      fprintf(stderr, "usage: %s <can_interface_battery> <can_interface_inverter>\n", argv[0]);
      return 1;
   }

   battery_socket = openSocket(argv[1]);
   inverter_socket = openSocket(argv[2]);

   const unsigned MAX_EVENTS = 10;
   struct epoll_event ev, events[MAX_EVENTS];
   int nfds, epollfd;

   epollfd = epoll_create1(0);
   if (epollfd == -1) {
      perror("epoll_create1");
      exit(EXIT_FAILURE);
   }

   ev.events = EPOLLIN;
   ev.data.fd = battery_socket;
   if (epoll_ctl(epollfd, EPOLL_CTL_ADD, battery_socket, &ev) == -1) {
      perror("epoll_ctl: can socket");
      exit(EXIT_FAILURE);
   }
   ev.data.fd = inverter_socket;
   if (epoll_ctl(epollfd, EPOLL_CTL_ADD, inverter_socket, &ev) == -1) {
      perror("epoll_ctl: can socket");
      exit(EXIT_FAILURE);
   }
   
   core::EpollTimer timer(epollfd);

   OutputPin positive_relay(0, 5, "relay_pos");
   OutputPin negative_relay(0, 6, "relay_neg");
   OutputPin indicator_led(0, 4, "led1");
   contactor::Nissan::LeafContactor contactor(
         timer,
         positive_relay,
         negative_relay,
         indicator_led,
         &std::cout);

   monitor::Nissan::Monitor monitor(contactor);
   can::services::Nissan::MessageFactory battery_sink(monitor, &std::cout);
   can::services::Nissan::FrameAggregator aggregator(battery_sink);
   CanSender battery_sender(battery_socket, "BAT", color::blue);
   can::services::Nissan::GroupPoller poller(battery_sender, timer);

   CanSender inverter_sender(inverter_socket, "INV", color::green);
   
//   inverter::SMA::SunnyBoyStorage inverter(inverter_sender, timer, monitor, contactor);
//   can::services::SMA::MessageFactory inverter_sink(inverter, &std::cout);

   inverter::TSUN::TSOL_H50K inverter(inverter_sender, timer, monitor, contactor);
   TsunSink inverter_sink(inverter);

   while (1)
   {
      nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
      if (nfds == -1) {
          perror("epoll_wait");
          exit(EXIT_FAILURE);
      }

      for (int n = 0; n < nfds; ++n)
      {
         if (events[n].data.fd == battery_socket ||
             events[n].data.fd == inverter_socket)
         {
            struct can_frame frame;
            int nbytes = read(events[n].data.fd, &frame, sizeof(struct can_frame));

            if (nbytes < 0) {
                    perror("can raw socket read");
                    return 1;
            }

            /* paranoid check ... */
            if (nbytes < int(sizeof(struct can_frame))) {
                    fprintf(stderr, "read: incomplete CAN frame\n");
                    return 1;
            }
            
            can::StandardDataFrame f(frame.can_id, frame.data, frame.can_dlc);

            if (events[n].data.fd == battery_socket)
            {            
               poller.received(f);
               aggregator.sink(f);
            }
            else
            {
               inverter_sink.sink(f);
            }
         }
         else
         {
            reinterpret_cast<core::EpollHandler*>(events[n].data.ptr)->handle();
         }
      }
   }

   return 0;
}
