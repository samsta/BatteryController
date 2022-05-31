/* SPDX-License-Identifier: GPL-3.0-or-later */

/*
 * This file is prototyping code - don't judge.
 */

/* commands for starting virtual can bus on command line
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0

* command for real can
sudo /sbin/ip link set can0 up type can bitrate 250000
sudo /sbin/ip link set can1 up type can bitrate 250000
sudo ip link set can0 txqueuelen 1000
sudo ip link set can1 txqueuelen 1000

* other
cd niallGit/BatteryController/build
make mitm
./mitm can1 can0 1
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
#include <unordered_map>

#include <chrono>
#include <iostream>
#include <sys/time.h>
#include <ctime>

uint32_t serial_number = 0;
std::set<canid_t> filter;

struct uint64x2
{
   uint64_t mask;
   uint64_t current_val;
} logging_data;
std::unordered_map<canid_t, uint64x2> logging_map;

struct replacedata
{
   uint32_t skipcount;
   uint32_t skippedcount;
   uint32_t sendcount;
   uint32_t sentcount;
   uint8_t replacemsgdata[8];
} replace_data;
std::unordered_map<canid_t, replacedata> replace_map;

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

   //--------------------------------------------------------
   // read logging filter config file
   char logfilter[] = "mitm-log-filter.txt";
   std::ifstream infile;
   char textin[10];
   infile.open(logfilter, std::ifstream::in);
   if (!infile)
   {
      std::cout << "logging filter configuration file not found: " << logfilter << std::endl;
      exit(EXIT_FAILURE);
   }

   std::cout << "Log following IDs when masked bits change" << std::endl;
   uint count = 0;
   canid_t canid;
   uint64_t logmask;
   // read each text from the file individually,format hex  canid byte7 ... byte0
   // store in sets canid and mask ('cause I can't figure out how to make a set of can_frame)
   while (infile >> textin)
   {
      if (count % 9 == 0)
      {
         logmask = 0;
         canid = strtol(textin, NULL, 16);
         //logging_canid.insert(canid);
         std::cout << std::hex << std::uppercase << canid << std::dec << " : ";
      }
      else {
         uint dex = (count % 9) - 1;
         uint64_t bits64 = strtol(textin, NULL, 16);
         if (serial_number > 0) std::cout << bits64 << " : ";
         logmask |= (bits64 << ((7-dex) * 8));
         if (serial_number > 0) std::cout << std::hex << logmask << "::" << std::dec << std::endl;
         if (dex == 7)
         {
            logging_data.mask = logmask;
            logging_data.current_val = 0;
            logging_map.insert({canid, logging_data });
            std::cout << std::setfill('0') << std::setw(16) << std::hex << logmask << std::dec << std::endl;
         }
      }
      count++;
   }
   infile.close();
   if (count % 9 != 0)
   {
      std::cout << std::endl << "incomplete logging filter configuration read from: " << logfilter << std::endl;
      exit(EXIT_FAILURE);
   }

   //--------------------------------------------------------
   // read message replace config file
   char msgrepconfig[] = "mitm-msg-replace.txt";
   //std::ifstream infile;
   //char textin[10];
   infile.open(msgrepconfig, std::ifstream::in);
   if (!infile)
   {
      std::cout << "message replace configuration file not found: " << msgrepconfig << std::endl;
      exit(EXIT_FAILURE);
   }

   std::cout << "Message replace IDs" << std::endl;
   count = 0;
   uint32_t skipcount, sendcount;
   // read each text from the file individually,format hex  canid skip-count send-count byte7 ... byte0
   // store in sets canid and mask ('cause I can't figure out how to make a set of can_frame)
   while (infile >> textin)
   {
      if (count % 11 == 0)
      {
         canid = strtol(textin, NULL, 16);
         std::cout << std::hex << std::uppercase << canid << std::dec << " : skip ";
      }
      else if (count % 11 == 1)
      {
         skipcount = strtol(textin, NULL, 10);
         std::cout << std::uppercase << skipcount << " : send ";
      }
      else if (count % 11 == 2)
      {
         sendcount = strtol(textin, NULL, 10);
         std::cout << std::uppercase << sendcount << " : msg ";
      }
      else {
         uint dex = (count % 11) - 3;
         if (serial_number > 0) std::cout << dex << " : ";
         uint32_t bits32 = strtol(textin, NULL, 16);
         if (serial_number > 0) std::cout << bits32 << " : ";
         replace_data.replacemsgdata[dex] = (uint8_t)bits32;
         std::cout << std::hex << (uint32_t)replace_data.replacemsgdata[dex] << std::dec << " ";
         if (dex == 7)
         {
            replace_data.skipcount = skipcount;
            replace_data.sendcount = sendcount;
            replace_data.sentcount = 0;
            replace_data.skippedcount = 0;
            replace_map.insert({canid, replace_data });
            std::cout << std::endl;
         }
      }
      count++;
   }
   infile.close();
   if (count % 11 != 0)
   {
      std::cout << std::endl << "incomplete logging filter data read from: " << logfilter << std::endl;
      exit(EXIT_FAILURE);
   }

   //--------------------------------------------------------
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

   // open log file
   char logfile[] = "mitm-log-file.csv";
   std::ofstream outfile;
   outfile.open(logfile, std::ios::trunc | std::ios::out);
   if (!outfile)
   {
      std::cout << "log file failed to open: " << logfile << std::endl;
      exit(EXIT_FAILURE);
   }
   outfile << "Time Stamp,ID,Extended,Dir,Bus,LEN,D1,D2,D3,D4,D5,D6,D7,D8" << std::endl;
   //outfile.close();

   // set the start time
   auto usec_since_epoch_start = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
   //std::cout << "microseconds since epoch: " << usec_since_epoch_start << std::endl;

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

         // see if we need to replace this message
         std::unordered_map<canid_t,replacedata>::iterator mapfindmsgrep = replace_map.find(frame.can_id);

         // I spend 3 fekking hours getting the next line to compile, don't know what I did to get it to work
         if (mapfindmsgrep != replace_map.end()) 
         {
            if (serial_number > 0) std::cout << std::hex << frame.can_id << ": Msg Rep : " << std::endl;

            mapfindmsgrep->second.skippedcount++;
            if (serial_number > 0) std::cout << "skippedcount : " << mapfindmsgrep->second.skippedcount << std::endl;

            if (mapfindmsgrep->second.skippedcount > mapfindmsgrep->second.skipcount)
            {
               if ((mapfindmsgrep->second.sendcount == 0) || (mapfindmsgrep->second.sentcount < mapfindmsgrep->second.sendcount))
               {
                  if (mapfindmsgrep->second.sendcount > 0) mapfindmsgrep->second.sentcount++;
                  for (uint i=0; i<sizeof(frame.data); i++)
                  {
                     frame.data[i] = mapfindmsgrep->second.replacemsgdata[i];
                  }
               }
            }
         }

         // send the can message on the appropriate bus
         if (events[n].data.fd == s1)
         {
            redirect(s2, frame);
         }
         else
         {
            redirect(s1, frame);
         }

         // check if this canid is one we should be logging
         std::unordered_map<canid_t,uint64x2>::iterator mapfind = logging_map.find(frame.can_id);

         // I spend 3 fekking hours getting the next line to compile, don't know what I did to get it to work
         if (mapfind != logging_map.end()) 
         {
            // this is a canid of note
            // get frame data into a uint64_t
            uint64_t framedata=0, eachbyte;
            for (int i=0; i<8; i++)
            {
               eachbyte = frame.data[i];
               framedata |= (eachbyte << ((7-i) * 8));
            }
            if (serial_number > 1) std::cout << "framedata : " << std::setfill('0') << std::setw(16) << std::hex << framedata << std::dec << std::endl;

            // see if the relevent bits have changed
            if ((mapfind->second.current_val ^ framedata) & mapfind->second.mask)
            {
               // relevant bits have changed
               mapfind->second.current_val = framedata;
               if (serial_number > 0) std::cout << std::hex << frame.can_id << ": Data has changed."  << std::dec << std::endl;

               // write to log file
               // Time Stamp,ID,Extended,Dir,Bus,LEN,D1,D2,D3,D4,D5,D6,D7,D8
               // 5637158,00000679,false,Rx,1,1,00,00,00,00,00,00,00,00,   <---- comma at the end of the line
               auto usec_since_epoch = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
               outfile << (usec_since_epoch - usec_since_epoch_start) << "," << std::setfill('0') << std::setw(8) << std::hex << frame.can_id << std::dec;
               if (events[n].data.fd == s1)
               {
                  outfile << ",false,Rx,1,8,";
               }
               else
               {
                  outfile << ",false,Tx,1,8,";
               }

               for (int i=0; i<8; i++) 
               {
                  char that[50]; sprintf(that, "%02x,",frame.data[i]);
                  outfile << that;
               }
               outfile << std::endl;

               if (serial_number > 1)
               {
                  std::cout << (usec_since_epoch - usec_since_epoch_start) << "," << std::setfill('0') << std::setw(8) << std::hex << frame.can_id << std::dec;
                  std::cout << ",false,Rx,1,8,";
                  for (int i=0; i<8; i++) 
                  {
                     char that[50]; sprintf(that, "%02x,",frame.data[i]);
                     std::cout << that;
                  }
                  std::cout << std::endl;
               }
            }

         }


      }
   }
   outfile.close();

   return 0;
}
