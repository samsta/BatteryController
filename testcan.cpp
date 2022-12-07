/* SPDX-License-Identifier: GPL-3.0-or-later */

/*
 * This file is prototyping code - don't judge.
 */

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <fstream>
#include <vector>

#include "packs/Nissan/LeafPack.hpp"
#include "packs/Nissan/LeafMultiPack.hpp"
#include "can/services/SMA/MessageFactory.hpp"
#include "can/services/TSUN/MessageFactory.hpp"
#include "inverter/SMA/SunnyBoyStorage.hpp"
#include "inverter/TSUN/TSOL-H50K.hpp"

#include "contactor/Nissan/LeafContactor.hpp"
#include "core/LibGpiod/OutputPin.hpp"
#include "core/SocketCan/CanPort.hpp"
#include "core/Linux/EpollTimer.hpp"
#include "core/Timer.hpp"
#include "core/USBInterface/USBInterface.hpp"
#include "core/Linux/ConsolePresenter.hpp"
#include "logging/colors.hpp"

#include <signal.h>

#define CONSOLE
// #undef CONSOLE

using namespace core::libgpiod;
namespace color = logging::color::ansi;

namespace {
   // this code required to catch ctrl-c and cleanly exit the program (open contactors)
  volatile sig_atomic_t keep_on_trucking = true;
  void handle_break(int thissig) {
    if (thissig == SIGINT) keep_on_trucking = false;
    std::cout << "ctrl-c pressed." << std::endl;
  }
}

int main(int argc, const char** argv)
{
   logging::ostream* log = &std::cout;
   std::ofstream logfile;
   
   // this code required to catch ctrl-c and cleanly exit the program (open contactors)
   struct sigaction action;
   memset(&action, 0, sizeof(struct sigaction));
   action.sa_handler = handle_break;
   sigaction(SIGINT, &action, NULL);
   sigset_t all_signals;
   sigemptyset(&all_signals);
   sigaddset(&all_signals, SIGINT);

   if (argc != 3 and argc != 4)
   {
      fprintf(stderr, "usage: %s <can_interface_inverter> <usb_port_battery123> [<usb_port_battery456]>\n", argv[0]);
      return 1;
   }

   const unsigned MAX_EVENTS = 10;
   struct epoll_event events[MAX_EVENTS];
   int nfds;
   int epollfd;

   epollfd = epoll_create1(0);
   if (epollfd == -1) {
      perror("epoll_create1");
      exit(EXIT_FAILURE);
   }

   // core::USBPort usb_port1(argv[2], epollfd);
   // core::USBPort usb_port2(argv[3], epollfd);
   core::USBPort usb_port2(argv[2], epollfd);

   core::CanPort inverter_port(argv[1], epollfd);
   core::EpollTimer timer(epollfd);

   OutputPin positive_relay_1(0, 5, "relay_pos_1");
   OutputPin negative_relay_1(0, 6, "relay_neg_1");
   OutputPin indicator_led_1(0, 4, "led_1");

   std::vector<monitor::Monitor*> vbatterymon;

   #ifdef CONSOLE
   core::ConsolePresenter console(timer, vbatterymon);
   if (console.isOperational())
   {
      logfile.open("log.txt");
      if (logfile.good())
      {
         log = &logfile;
      }
      else
      {
         std::cerr << "Failed opening logfile log.txt: " << strerror(errno) << std::endl;
      }
   }
   else
   {
      std::cerr << "Don't have a terminal to run console presenter, so I'll proceed logging to stdout" << std::endl;
   }
   #endif

   // packs::Nissan::LeafPack battery_pack_1(
   //       usb_port1.getSinkOutbound(0),
   //       timer,
   //       log);

   // packs::Nissan::LeafPack battery_pack_2(
   //      usb_port1.getSinkOutbound(1),
   //      timer,
   //      log);

   // packs::Nissan::LeafPack battery_pack_3(
   //      usb_port1.getSinkOutbound(2),
   //      timer,
   //      log);

   // packs::Nissan::LeafPack battery_pack_4(
   //      usb_port2.getSinkOutbound(0),
   //      timer,
   //      log);

   // packs::Nissan::LeafPack battery_pack_5(
   //      usb_port2.getSinkOutbound(1),
   //      timer,
   //      log);

   // packs::Nissan::LeafPack battery_pack_6(
   //      usb_port2.getSinkOutbound(2),
   //      timer,
   //      log);

   packs::Nissan::LeafPack battery_pack_6(
        usb_port2.getSinkOutbound(0),
        timer,
        log);
   
   vbatterymon.push_back( &battery_pack_6.getMonitor());

   // std::vector<monitor::Monitor*> vbatterymon = {
   //          // &battery_pack_1.getMonitor(),
   //          // &battery_pack_2.getMonitor(),
   //          // &battery_pack_3.getMonitor(),
   //          // &battery_pack_4.getMonitor(),
   //          // &battery_pack_5.getMonitor(),
   //          &battery_pack_6.getMonitor()};
   std::vector<contactor::Contactor*> vbatterycon = {
            // &battery_pack_1.getContactor(),
            // &battery_pack_2.getContactor(),
            // &battery_pack_3.getContactor(),
            // &battery_pack_4.getContactor(),
            // &battery_pack_5.getContactor(),
            &battery_pack_6.getContactor()};

   // usb_port1.setupLogger(*log, "<USB1 OUT>", color::cyan);
   usb_port2.setupLogger(*log, "<USB2 OUT>", color::cyan);

   // usb_port1.setSinkInbound(0, battery_pack_1);
   // usb_port1.setSinkInbound(1, battery_pack_2);
   // usb_port1.setSinkInbound(2, battery_pack_3);

   // usb_port2.setSinkInbound(0, battery_pack_4);
   // usb_port2.setSinkInbound(1, battery_pack_5);
   // usb_port2.setSinkInbound(2, battery_pack_6);
   usb_port2.setSinkInbound(0, battery_pack_6);

   packs::Nissan::LeafMultiPack multi_battery(
                     vbatterymon,
                     vbatterycon,
                     timer,
                     positive_relay_1,
                     negative_relay_1,
                     indicator_led_1,
                     log);

   inverter::TSUN::TSOL_H50K inverter(
         inverter_port,
         timer,
         multi_battery,
         multi_battery.getMainContactor());
   can::services::TSUN::MessageFactory inverter_message_factory(inverter, log);

   inverter_port.setupLogger(*log, "<INV OUT>", color::green);
   inverter_port.setSink(inverter_message_factory);

   #ifdef CONSOLE
   if (console.isOperational())
   {
      console.setMonitor(vbatterymon);
      console.setContactor(multi_battery.getMainContactor());
   }
   #endif

   while (keep_on_trucking)
   {
      sigprocmask(SIG_BLOCK, &all_signals, NULL);
      nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
      sigprocmask(SIG_UNBLOCK, &all_signals, NULL);
      if (nfds == -1) {
         perror("epoll_wait error");
         keep_on_trucking = false;
      }

      for (int n = 0; n < nfds; ++n)
      {
         reinterpret_cast<core::EpollHandler*>(events[n].data.ptr)->handle();
      }
   }

   std::cout << "Program EXIT."  << std::endl;

   return 0;
}
