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

#include "can/StandardDataFrame.hpp"
#include "can/services/Nissan/FrameAggregator.hpp"
#include "can/services/Nissan/GroupPoller.hpp"
#include "can/messages/Tesla/DetailedCellData.hpp"
#include "can/messages/Nissan/CellVoltages.hpp"
#include "can/messages/Nissan/PackTemperatures.hpp"
#include "can/messages/Nissan/BatteryState.hpp"
#include "can/messages/Nissan/Ids.hpp"

class PrintingSink: public can::FrameSink
{
public:
   virtual void sink(const can::DataFrame& f)
   {
      if (f.id() == 0x7bb)
      {
         can::messages::Nissan::CellVoltages voltages(f);

         if (voltages.valid())
         {
            std::cout << "<IN>  " << voltages << std::endl;
            return;
         }

         can::messages::Nissan::PackTemperatures temperatures(f);
         if (temperatures.valid())
         {
            std::cout << "<IN>  " << temperatures << std::endl;
            return;
         }

         can::messages::Nissan::BatteryState state(f);
         if (state.valid())
         {
            std::cout << "<IN>  " << state << std::endl;
            return;
         }
      }
   }
};

class CanSender: public can::FrameSink
{
public:
   CanSender(int fd): m_fd(fd)
   {
   }

   virtual void sink(const can::DataFrame& f)
   {
      std::cout << "<OUT> " << f << std::endl;

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
};

int main(int argc, const char** argv)
{
   int s, timer;
   struct sockaddr_can addr;
   struct ifreq ifr;

   if (argc != 2)
   {
      fprintf(stderr, "usage: %s <can_interface>\n", argv[0]);
      return 1;
   }

   s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

   strcpy(ifr.ifr_name, argv[1] );
   ioctl(s, SIOCGIFINDEX, &ifr);

   addr.can_family = AF_CAN;
   addr.can_ifindex = ifr.ifr_ifindex;

   bind(s, (struct sockaddr *)&addr, sizeof(addr));

   struct can_filter rfilter[2];
   rfilter[0].can_id   = 0x7bb;
   rfilter[0].can_mask = CAN_SFF_MASK;
   rfilter[1].can_id   = 0x6f2;
   rfilter[1].can_mask = CAN_SFF_MASK;

   setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));


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

   timer = timerfd_create(CLOCK_MONOTONIC, 0);
   if (timer == -1)
   {
      perror("timerfd_create");
      exit(EXIT_FAILURE);
   }

   struct itimerspec its = itimerspec();
   its.it_interval.tv_nsec = 500000000;
   its.it_value.tv_nsec = 500000000;
   timerfd_settime(timer, 0, &its, NULL);

   ev.events = EPOLLIN;
   ev.data.fd = timer;
   if (epoll_ctl(epollfd, EPOLL_CTL_ADD, timer, &ev) == -1) {
      perror("epoll_ctl: timer");
      exit(EXIT_FAILURE);
   }

   PrintingSink printing_sink;
   can::services::Nissan::FrameAggregator aggregator(printing_sink);
   CanSender sender(s);
   can::services::Nissan::GroupPoller poller(sender);

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
                    return 1;
            }

            /* paranoid check ... */
            if (nbytes < int(sizeof(struct can_frame))) {
                    fprintf(stderr, "read: incomplete CAN frame\n");
                    return 1;
            }

            can::StandardDataFrame f(frame.can_id, frame.data, frame.can_dlc);

            can::messages::Tesla::DetailedCellData d(f);
            if (d.valid())
            {
               std::cout << d << std::endl;
            }

            poller.received(f);
            aggregator.sink(f);
         }
         else if (events[n].data.fd == timer)
         {
            poller.poll();
            uint64_t num_expirations;
            (void)read(timer, &num_expirations, sizeof(num_expirations));
         }
      }
   }

   return 0;
}
