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
// #include "can/services/SMA/MessageFactory.hpp"
#include "can/services/TSUN/MessageFactory.hpp"
// #include "inverter/SMA/SunnyBoyStorage.hpp"
// #include "inverter/TSUN/TSOL-H50K.hpp"
#include "can/services/SINEX/MessageFactory.hpp"
#include "inverter/SINEX/SE-PWS2.hpp"

#include "contactor/Nissan/LeafContactor.hpp"
#include "core/LibGpiod/OutputPin.hpp"
#include "core/SocketCan/CanPort.hpp"
#include "core/Linux/EpollTimer.hpp"
#include "core/Timer.hpp"
#include "core/USBInterface/USBInterface.hpp"
#include "core/Linux/ConsolePresenter.hpp"
#include "logging/colors.hpp"
#include "logging/logging.hpp"
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
   std::ofstream logfile;
   std::ostringstream ss;

   // this code required to catch ctrl-c and cleanly exit the program (open contactors)
   struct sigaction action;
   memset(&action, 0, sizeof(struct sigaction));
   action.sa_handler = handle_break;
   sigaction(SIGINT, &action, NULL);
   sigset_t all_signals;
   sigemptyset(&all_signals);
   sigaddset(&all_signals, SIGINT);

   if (argc != 5)
   {
      fprintf(stderr, "usage: %s <logging level 1-6> <can_interface_inverter> <usb_port_battery123> [<usb_port_battery456]>\n", argv[0]);
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
   core::EpollTimer timer(epollfd);
   std::vector<monitor::Monitor*> vbatterymon;

   // create the logging object
   logging::Logger logger((logging::LOG_LEVEL) atoi(argv[1]), timer, vbatterymon );
   timer.setLogger(&logger);
   logger.info("------------------- BatteryController Started ------------------- ",__FILENAME__, __LINE__);
   std::string smsg;
   smsg.append("LOGGER_LEVEL: ");
   smsg.append(argv[1]);
   logger.info(smsg);

   core::USBPort usb_port1(argv[3], epollfd, &logger);
   // core::USBPort usb_port2(argv[4], epollfd, &logger);

   // core::CanPort inverter_port(argv[2], epollfd, &logger);

   OutputPin pre_charge_relay_1(0, 4, "relay_prechg_1",core::OutputPin::HIGH);
   OutputPin positive_relay_1(0, 5, "relay_pos_1",core::OutputPin::HIGH);
   OutputPin negative_relay_1(0, 6, "relay_neg_1",core::OutputPin::HIGH);

   #ifdef CONSOLE
   core::ConsolePresenter console(timer, vbatterymon);
   if (!console.isOperational())
   {
      std::cerr << "Don't have a terminal to run console presenter" << std::endl;
   }
   #endif

   // **********
   // 1 teensy 1 battery 1 inverter
   // **********
   char BP1[] = "BP1";
   packs::Nissan::LeafPack battery_pack_1( BP1,
        usb_port1.getSinkOutbound(0),
        timer,
        &logger);
   vbatterymon.push_back( &battery_pack_1.getMonitor());

   // char BP2[] = "BP2";
   // packs::Nissan::LeafPack battery_pack_2( BP2,
   //      usb_port1.getSinkOutbound(1),
   //      timer,
   //      &logger);
   // vbatterymon.push_back( &battery_pack_2.getMonitor());

   // char BP3[] = "BP3";
   // packs::Nissan::LeafPack battery_pack_3( BP3,
   //      usb_port1.getSinkOutbound(2),
   //      timer,
   //      &logger);
   // vbatterymon.push_back( &battery_pack_3.getMonitor());

   // char BP4[] = "BP4";
   // packs::Nissan::LeafPack battery_pack_4( BP4,
   //      usb_port2.getSinkOutbound(0),
   //      timer,
   //      &logger);
   // vbatterymon.push_back( &battery_pack_4.getMonitor());

   // char BP5[] = "BP5";
   // packs::Nissan::LeafPack battery_pack_5( BP5,
   //      usb_port2.getSinkOutbound(1),
   //      timer,
   //      &logger);
   // vbatterymon.push_back( &battery_pack_5.getMonitor());

   std::vector<contactor::Contactor*> vbatterycon = {
            &battery_pack_1.getContactor()
            // ,
            // &battery_pack_2.getContactor()
            // ,
            // &battery_pack_3.getContactor()
            // ,
            // &battery_pack_4.getContactor()
            // ,
            // &battery_pack_5.getContactor()
            };

   usb_port1.setSinkInbound(0, battery_pack_1.getPackName(), battery_pack_1);
   // usb_port1.setSinkInbound(1, battery_pack_2.getPackName(), battery_pack_2);
   // usb_port1.setSinkInbound(2, battery_pack_3.getPackName(), battery_pack_3);
   // usb_port2.setSinkInbound(0, battery_pack_4.getPackName(), battery_pack_4);
   // usb_port2.setSinkInbound(1, battery_pack_5.getPackName(), battery_pack_5);

   packs::Nissan::LeafMultiPack multi_pack(
                     vbatterymon,
                     vbatterycon,
                     timer,
                     positive_relay_1,
                     negative_relay_1,
                     pre_charge_relay_1,
                     &logger);
   // add the multipack to the battery list
   // multi_pack (above) will not see this addition
   // logger and console (below) will see it
   vbatterymon.push_back( &multi_pack);

   // inverter::TSUN::TSOL_H50K inverter(
   inverter::SINEX::SE_PWS2 inverter(
         usb_port1.getSinkOutbound(1),
         timer,
         multi_pack,
         multi_pack.getMainContactor(),
         &logger);
   can::services::TSUN::MessageFactory inverter_message_factory(inverter, &logger);
   // can::services::SINEX::MessageFactory inverter_message_factory(inverter, &logger);
   // inverter_port.setSink(inverter_message_factory);
   char inverter_name[] = "Inverter";
   usb_port1.setSinkInbound(1, inverter_name, inverter_message_factory);

   logger.setMonitor(vbatterymon);

   #ifdef CONSOLE
   if (console.isOperational())
   {
      console.setMonitor(vbatterymon);
      console.setContactor(multi_pack.getMainContactor());
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
   logger.info("Program EXIT.");
   logger.info("------------------- BatteryController STOPPED ------------------- ");

   return 0;
}
