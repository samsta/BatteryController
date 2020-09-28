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
#include <deque>


#include "can/StandardDataFrame.hpp"
#include "can/messages/SMA/BatteryAlarms.hpp"
#include "can/messages/SMA/BatteryErrors.hpp"
#include "can/messages/SMA/BatteryEvents.hpp"
#include "can/messages/SMA/BatteryLimits.hpp"
#include "can/messages/SMA/BatteryMeasurements.hpp"
#include "can/messages/SMA/BatteryState.hpp"
#include "can/messages/SMA/BatterySystemInfo.hpp"

using namespace can::messages::SMA;

std::deque<can::DataFrame*> send_queue;

void sendUpdateBlockResponse()
{
   static can::StandardDataFrame update_block_response("758#0762000001080000");
   send_queue.push_front(&update_block_response);
   std::cout << "queued update block response" << std::endl;
}

void sendBatteryIdentification()
{
   static can::StandardDataFrame battery_id[] = {
      BatterySystemInfo().setVersion(370673156)
                         .setCapacityKwh(9.8)
                         .setNumberOfModules(1)
                         .setManufacturerId(2),
      can::StandardDataFrame("598#12345678FFFFFFFF"), // BatteryIdentity
      can::StandardDataFrame("5d8#004D6F6E6B65794C"), // BatteryManufacturer MonkeyL
      can::StandardDataFrame("5d8#0161620000000000"), // BatteryManufacturer ab
      can::StandardDataFrame("618#0041706553686974"), // BatteryName ApeShit
      can::StandardDataFrame("618#012044656C757865")  // BatteryName  Deluxe
   };
   for (int k = sizeof(battery_id)/sizeof(*battery_id)-1; k >=0; k--)
   {
      send_queue.push_front(&battery_id[k]);
   }
   std::cout << "queued battery identification messages" << std::endl;
}

void sendPeriodicData()
{
   static can::StandardDataFrame periodic_data[] = {
      BatteryMeasurements().setVoltage(469.9)
                           .setCurrent(1.6)
                           .setTemperature(14.1)
                           .setState(3)
                           .setInverterControlFlags(0),
      BatteryLimits().setChargeVoltage(495)
                     .setDischargeVoltage(435)
                     .setChargeCurrent(16.2)
                     .setDischargeCurrent(18.7),
      BatteryState().setSocPercent(54.5)
                    .setSohPercent(87.8)
                    .setEnergyRemainingKwh(46.6)
                    .setFullChargedEnergyKwh(82.3),
      BatteryAlarms(),
      BatteryErrors(),
      BatteryEvents()
   };
   // periodic data goes to the back of the queue, so we can respond to to
   //  requests from inverter more quickly
   for (unsigned k = 0; k < sizeof(periodic_data)/sizeof(*periodic_data); k++)
   {
      send_queue.push_back(&periodic_data[k]);
   }
   std::cout << "queued periodic battery messages" << std::endl;
}

int main(int argc, const char** argv)
{
   int s, timer, refill_timer;
   struct sockaddr_can addr;
   struct ifreq ifr;

   if (argc != 2)
   {
      fprintf(stderr, "usage: %s <can_interface>\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

   strcpy(ifr.ifr_name, argv[1] );
   if (ioctl(s, SIOCGIFINDEX, &ifr) != 0)
   {
      perror("ioctl on socket");
      exit(EXIT_FAILURE);
   }

   addr.can_family = AF_CAN;
   addr.can_ifindex = ifr.ifr_ifindex;

   if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) != 0)
   {
      perror("binding to socket");
      exit(EXIT_FAILURE);
   }

   const unsigned MAX_EVENTS = 2;
   struct epoll_event ev, events[MAX_EVENTS];
   int nfds, epollfd;

   epollfd = epoll_create1(0);
   if (epollfd == -1) {
      perror("epoll_create1");
      exit(EXIT_FAILURE);
   }

   ev.events = EPOLLIN;
   ev.data.fd = s;
   if (epoll_ctl(epollfd, EPOLL_CTL_ADD, s, &ev) == -1) {
      perror("epoll_ctl: can socket");
      exit(EXIT_FAILURE);
   }

   // timer to send out messages - if any
   timer = timerfd_create(CLOCK_MONOTONIC, 0);
   if (timer == -1)
   {
      perror("timerfd_create");
      exit(EXIT_FAILURE);
   }

   // timer to refill the message queue with periodic messages
   refill_timer = timerfd_create(CLOCK_MONOTONIC, 0);
   if (timer == -1)
   {
      perror("timerfd_create");
      exit(EXIT_FAILURE);
   }

   {
      struct itimerspec its = itimerspec();
      its.it_interval.tv_nsec = 10000000;
      its.it_value.tv_nsec = 10000000;
      timerfd_settime(timer, 0, &its, NULL);
   }
   
   ev.events = EPOLLIN;
   ev.data.fd = timer;
   if (epoll_ctl(epollfd, EPOLL_CTL_ADD, timer, &ev) == -1) {
      perror("epoll_ctl: timer");
      exit(EXIT_FAILURE);
   }

   {
      struct itimerspec its = itimerspec();
      its.it_interval.tv_sec = 5;
      its.it_value.tv_sec = 5;
      timerfd_settime(refill_timer, 0, &its, NULL);
   }

   ev.events = EPOLLIN;
   ev.data.fd = refill_timer;
   if (epoll_ctl(epollfd, EPOLL_CTL_ADD, refill_timer, &ev) == -1) {
      perror("epoll_ctl: contactor_timer");
      exit(EXIT_FAILURE);
   }


   
   std::cout << "Emulator ready on " << argv[1] << std::endl;
   while (1)
   {
      nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
      if (nfds == -1) {
          perror("epoll_wait");
          exit(EXIT_FAILURE);
      }

      for (int n = 0; n < nfds; ++n)
      {
         if (events[n].data.fd == s)
         {
            struct can_frame frame;
            int nbytes = read(s, &frame, sizeof(struct can_frame));
            
            if (nbytes < 0) {
                    perror("can raw socket read");
                    continue;
            }

            switch (frame.can_id)
            {
            case 0x720: // UpdateBlockRequest2
               sendUpdateBlockResponse();
               break;
            case 0x5E0: // InverterManufacturer
               sendBatteryIdentification();
               break;
            default:
               // ignore anything else
               break;
            }
         }
         else if (events[n].data.fd == timer)
         {
            if (not send_queue.empty())
            {
               can::DataFrame& f(*send_queue.front());
               send_queue.pop_front();
               
               struct can_frame frame;
               frame.can_id = f.id();
               frame.can_dlc = f.size();
               std::copy(f.begin(), f.end(), frame.data);
               
               if (write(s, &frame, sizeof(frame)) != sizeof(frame))
               {
                  perror("Writing CAN frame");
               }
            }
            uint64_t num_expirations;
            (void)read(timer, &num_expirations, sizeof(num_expirations));
         }
         else if (events[n].data.fd == refill_timer)
         {
            sendPeriodicData();
            uint64_t num_expirations;
            (void)read(refill_timer, &num_expirations, sizeof(num_expirations));
         }
      }
   }

   return 0;
}
