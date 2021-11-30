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
#include <sstream>
#include <fstream>

uint32_t serial_number = 0;
std::set<canid_t> filter;

std::set<canid_t> logging_canid;
std::set<int64_t> logging_data;
std::set<int64_t> logging_mask;



void redirect(int dest, struct can_frame& frame)
{
   if (filter.count(frame.can_id))
   {
      // drop frame
      return;
   }

   // if (frame.can_id == 0x598)
   // {
   //    // spoof serial number
   //    memcpy(frame.data, &serial_number, sizeof(serial_number));
   // }

   if (write(dest, &frame, sizeof(frame)) != sizeof(frame))
   {
      perror("Writing CAN frame");
   }
}

void processKeyboardInput()
{
   char operation;
   canid_t id;
   char buf[256] = {0};
   std::cin.getline(buf, sizeof(buf));

   int count = std::cin.gcount();
   if (count == 0) return;

   std::stringstream s(buf);
   s >> operation;
   s >> std::hex >> id;

   if (s.fail())
   {
      std::cout << "Can't extract CAN id from '" << s.str() << "'" << std::endl;
      return;
   }

   if (operation == '-')
   {
      if (filter.count(id))
      {
         std::cout << "Already dropping " << std::hex << std::setfill('0') << std::setw(3) << id << std::dec << std::endl;         
      }
      else
      {
          filter.insert(id);
          std::cout << "Going to drop " << std::hex << std::setfill('0') << std::setw(3) << id << std::dec << std::endl;
      }
   }
   else if (operation == '+')
   {
      if (filter.count(id))
      {     
         filter.erase(id);
         std::cout << "Won't drop " << std::hex << std::setfill('0') << std::setw(3) << id << std::dec << " anymore" << std::endl;
      }
      else
      {
         std::cout << "Wasn't dropping " << std::hex << std::setfill('0') << std::setw(3) << id << std::dec << " so no point in enabling it" << std::endl;
      }
   }
   else
   {
      std::cout << "Huh?! What did you mean by '" << operation << "'?" << std::endl;
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
   std::cout << "ver 1.1\n";

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




   // read logging filter data
   char logfilter[] = "mitm-log-filter.txt";
   std::ifstream infile;
   char textin[10];
   infile.open(logfilter, std::ifstream::in);
   if (!infile)
   {
      std::cout << "logging filter file not found: " << logfilter << std::endl;
      exit(EXIT_FAILURE);
   }


   std::cout << "Log following IDs when masked bits change" << std::endl;
   uint count = 0;
   canid_t canid;
   uint64_t logmask;
   while (infile >> textin)
   {
      if (count % 9 == 0)
      {
         logmask = 0;
         canid = strtol(textin, NULL, 16);
         logging_canid.insert(canid);
         std::cout << std::hex << std::uppercase << canid << std::dec << " : ";
      }
      else {
         uint dex = (count % 9) - 1;
         uint64_t bits64 = strtol(textin, NULL, 16);
         //std::cout << bits64 << ":";
         logmask |= (bits64 << ((7-dex) * 8));
         //std::cout << std::hex << logmask << "::" << std::dec << std::endl;
         if (dex == 7)
         {
            logging_mask.insert(logmask);
            std::cout << std::setfill('0') << std::setw(16) << std::hex << logmask << std::dec << std::endl;
         }
      }
      count++;
   }
   infile.close();
















   const unsigned MAX_EVENTS = 4;
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

   ev.data.fd = fileno(stdin);
   if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fileno(stdin), &ev) == -1) {
      perror("epoll_ctl: stdin");
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
         if (events[n].data.fd == fileno(stdin))
         {
            processKeyboardInput();
            continue;
         }

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
