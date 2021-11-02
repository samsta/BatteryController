/* SPDX-License-Identifier: GPL-3.0-or-later */

/*
 * This file is prototyping code - don't judge.
 */

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <fstream>

#include "packs/Nissan/LeafPack.hpp"
#include "can/services/SMA/MessageFactory.hpp"
#include "can/services/TSUN/MessageFactory.hpp"
#include "inverter/SMA/SunnyBoyStorage.hpp"
#include "inverter/TSUN/TSOL-H50K.hpp"

#include "contactor/Nissan/LeafContactor.hpp"
#include "core/LibGpiod/OutputPin.hpp"
#include "core/SocketCan/CanPort.hpp"
#include "core/Linux/EpollTimer.hpp"
#include "core/Timer.hpp"
#include "core/Linux/ConsolePresenter.hpp"
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
   
   std::ofstream log;
   log.open("log.txt");

   core::CanPort battery_port(argv[1], epollfd);
   core::CanPort inverter_port(argv[2], epollfd);
   core::EpollTimer timer(epollfd);

   OutputPin positive_relay(0, 5, "relay_pos");
   OutputPin negative_relay(0, 6, "relay_neg");
   OutputPin indicator_led(0, 4, "led1");

   packs::Nissan::LeafPack battery_pack(
         battery_port,
         timer,
         positive_relay,
         negative_relay,
         indicator_led,
         &log);

   battery_port.setupLogger(log, "<BAT OUT>", color::blue);
   battery_port.setSink(battery_pack);

   
//   inverter::SMA::SunnyBoyStorage inverter(inverter_sender, timer, monitor, contactor);
//   can::services::SMA::MessageFactory inverter_sink(inverter, &std::cout);

   inverter::TSUN::TSOL_H50K inverter(
         inverter_port,
         timer,
         battery_pack.getMonitor(),
         battery_pack.getContactor());
   can::services::TSUN::MessageFactory inverter_message_factory(inverter, &log);

   inverter_port.setupLogger(log, "<INV OUT>", color::green);
   inverter_port.setSink(inverter_message_factory);

   core::ConsolePresenter console(
         timer,
         battery_pack.getMonitor(),
         battery_pack.getContactor());

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
