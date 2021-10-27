/* SPDX-License-Identifier: GPL-3.0-or-later */

/*
 * This file is prototyping code - don't judge.
 */

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <algorithm>

#include "can/StandardDataFrame.hpp"
#include "can/services/Nissan/FrameAggregator.hpp"
#include "can/services/Nissan/GroupPoller.hpp"
#include "can/services/Nissan/MessageFactory.hpp"
#include "can/services/SMA/MessageFactory.hpp"
#include "can/services/TSUN/MessageFactory.hpp"
#include "monitor/Nissan/Monitor.hpp"
#include "inverter/SMA/SunnyBoyStorage.hpp"
#include "inverter/TSUN/TSOL-H50K.hpp"

#include "contactor/Nissan/LeafContactor.hpp"
#include "core/LibGpiod/OutputPin.hpp"
#include "core/SocketCan/CanPort.hpp"
#include "core/Linux/EpollTimer.hpp"
#include "core/Timer.hpp"
#include "core/Callback.hpp"
#include "logging/colors.hpp"

using namespace core::libgpiod;
namespace color = logging::color::ansi;

int main(int argc, const char** argv)
{
   if (argc != 3)
   {
      fprintf(stderr, "usage: %s <can_interface_battery> <can_interface_inverter>\n", argv[0]);
      return 1;
   }

   const unsigned MAX_EVENTS = 10;
   struct epoll_event events[MAX_EVENTS];
   int nfds, epollfd;

   epollfd = epoll_create1(0);
   if (epollfd == -1) {
      perror("epoll_create1");
      exit(EXIT_FAILURE);
   }
   
   core::CanPort battery_port(argv[1], epollfd);
   core::CanPort inverter_port(argv[2], epollfd);
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
   battery_port.setupLogger(std::cout, "<BAT OUT>", color::blue);
   can::services::Nissan::GroupPoller poller(battery_port, timer);

   class BatteryPack: public can::FrameSink
   {
   public:
      BatteryPack(can::services::Nissan::GroupPoller& p,
                  can::services::Nissan::FrameAggregator& a):
          poller(p),
          aggregator(a)
      {}

      virtual void sink(const can::DataFrame& f)
      {
         poller.received(f);
         aggregator.sink(f);
      }

      can::services::Nissan::GroupPoller& poller;
      can::services::Nissan::FrameAggregator& aggregator;
   } battery_pack(poller, aggregator);
   battery_port.setSink(battery_pack);

   inverter_port.setupLogger(std::cout, "<INV OUT>", color::green);
   
//   inverter::SMA::SunnyBoyStorage inverter(inverter_sender, timer, monitor, contactor);
//   can::services::SMA::MessageFactory inverter_sink(inverter, &std::cout);

   inverter::TSUN::TSOL_H50K inverter(inverter_port, timer, monitor, contactor);
   can::services::TSUN::MessageFactory inverter_sink(inverter, &std::cout);
   inverter_port.setSink(inverter_sink);

   while (1)
   {
      nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
      if (nfds == -1) {
          perror("epoll_wait");
          exit(EXIT_FAILURE);
      }

      for (int n = 0; n < nfds; ++n)
      {
         reinterpret_cast<core::EpollHandler*>(events[n].data.ptr)->handle();
      }
   }

   return 0;
}
