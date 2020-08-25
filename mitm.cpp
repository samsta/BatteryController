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
#include <set>
#include <iostream>
#include <iomanip>

uint32_t serial_number = 0;
std::set<canid_t> filter;

void redirect(int dest, struct can_frame& frame)
{
   if (filter.count(frame.can_id))
   {
      // drop frame
      return;
   }

   if (frame.can_id == 0x598)
   {
      // spoof serial number
      memcpy(frame.data, &serial_number, sizeof(serial_number));
   }

   if (write(dest, &frame, sizeof(frame)) != sizeof(frame))
   {
      perror("Writing CAN frame");
   }
}

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
   int s1, s2;

   if (argc < 4)
   {
      fprintf(stderr, "usage: %s <can_interface1> <can_interface2> <serial> [<id_to_drop1> .. <id_to_dropN>]\n", argv[0]);
      exit(EXIT_FAILURE);
   }

   s1 = openSocket(argv[1]);
   s2 = openSocket(argv[2]);

   serial_number = strtol(argv[3], NULL, 10);
   std::cout << "Going to spoof serial number " << serial_number << std::endl;

   for (int i=4; i < argc; i++)
   {
      canid_t id = strtol(argv[i], NULL, 16);
      filter.insert(id);
      std::cout << "Going to drop " << std::hex << std::setfill('0') << std::setw(3) << id << std::dec << std::endl;
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
   ev.data.fd = s1;
   if (epoll_ctl(epollfd, EPOLL_CTL_ADD, s1, &ev) == -1) {
      perror("epoll_ctl: can socket");
      exit(EXIT_FAILURE);
   }

   ev.data.fd = s2;
   if (epoll_ctl(epollfd, EPOLL_CTL_ADD, s2, &ev) == -1) {
      perror("epoll_ctl: can socket");
      exit(EXIT_FAILURE);
   }

   std::cout << "Man in the middle ready on " << argv[1] << " and " << argv[2] << std::endl;
   while (1)
   {
      nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
      if (nfds == -1) {
          perror("epoll_wait");
          exit(EXIT_FAILURE);
      }

      for (int n = 0; n < nfds; ++n)
      {
         struct can_frame frame;
         int nbytes = read(events[n].data.fd, &frame, sizeof(struct can_frame));
         
         if (nbytes < 0) {
            perror("can raw socket read");
            continue;
         }
         
         if (events[n].data.fd == s1)
         {
            redirect(s2, frame);
         }
         else
         {
            redirect(s1, frame);
         }
      }
   }

   return 0;
}
