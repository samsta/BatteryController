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
#include "core/USBInterface/USBInterface.hpp"
#include "core/Linux/ConsolePresenter.hpp"
#include "logging/colors.hpp"

#include <signal.h>

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

   if (argc != 4)
   {
      fprintf(stderr, "usage: %s <can_interface_battery> <can_interface_inverter> <usb_port_name>\n", argv[0]);
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

   core::USBPort usb_port(argv[3], epollfd);

   core::CanPort battery_port(argv[1], epollfd);
   core::CanPort inverter_port(argv[2], epollfd);
   core::EpollTimer timer(epollfd);

   OutputPin positive_relay(0, 5, "relay_pos");
   OutputPin negative_relay(0, 6, "relay_neg");
   OutputPin indicator_led(0, 4, "led1");

   // core::ConsolePresenter console(timer);
   // if (console.isOperational())
   // {
   //    logfile.open("log.txt");
   //    if (logfile.good())
   //    {
   //       log = &logfile;
   //    }
   //    else
   //    {
   //       std::cerr << "Failed opening logfile log.txt: " << strerror(errno) << std::endl;
   //    }
   // }
   // else
   // {
   //    std::cerr << "Don't have a terminal to run console presenter, so I'll proceed logging to stdout" << std::endl;
   // }
   
   packs::Nissan::LeafPack battery_pack(
         usb_port,
         timer,
         positive_relay,
         negative_relay,
         indicator_led,
         log);
//   packs::Nissan::LeafPack battery_pack(
//         battery_port,
//         timer,
//         positive_relay,
//         negative_relay,
//         indicator_led,
//         log);

   battery_port.setupLogger(*log, "<BAT OUT>", color::blue);
   battery_port.setSink(battery_pack);

   usb_port.setupLogger(*log, "<USB>", color::cyan);

   
//   inverter::SMA::SunnyBoyStorage inverter(inverter_sender, timer, monitor, contactor);
//   can::services::SMA::MessageFactory inverter_sink(inverter, &std::cout);

   inverter::TSUN::TSOL_H50K inverter(
         inverter_port,
         timer,
         battery_pack.getMonitor(),
         battery_pack.getContactor());
   can::services::TSUN::MessageFactory inverter_message_factory(inverter, log);

   inverter_port.setupLogger(*log, "<INV OUT>", color::green);
   inverter_port.setSink(inverter_message_factory);

   // if (console.isOperational())
   // {
   //    console.setMonitor(battery_pack.getMonitor());
   //    console.setContactor(battery_pack.getContactor());
   // }

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
