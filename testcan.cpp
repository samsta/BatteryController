#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>

/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "can/StandardDataFrame.hpp"
#include "can/services/Nissan/FrameAggregator.hpp"
#include "can/messages/Tesla/DetailedCellData.hpp"
#include "can/messages/Nissan/CellVoltages.hpp"
#include "can/messages/Nissan/PackTemperatures.hpp"
#include "can/messages/Nissan/BatteryState.hpp"

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
            std::cout << voltages << std::endl;
            return;
         }

         can::messages::Nissan::PackTemperatures temperatures(f);
         if (temperatures.valid())
         {
            std::cout << temperatures << std::endl;
            return;
         }

         can::messages::Nissan::BatteryState state(f);
         if (state.valid())
         {
            std::cout << state << std::endl;
            return;
         }
      }
   }
};

int main(int argc, const char** argv)
{
   int s;
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

   PrintingSink printing_sink;
   can::services::Nissan::FrameAggregator aggregator(printing_sink);

   struct can_frame frame;

   while (1)
   {
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

      aggregator.sink(f);
   }

   return 0;
}
