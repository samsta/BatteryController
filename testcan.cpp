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
#include <gpiod.h>
#include <errno.h>

#include "can/StandardDataFrame.hpp"
#include "can/services/Nissan/FrameAggregator.hpp"
#include "can/services/Nissan/GroupPoller.hpp"
#include "can/messages/Tesla/DetailedCellData.hpp"
#include "can/messages/Nissan/CellVoltageRange.hpp"
#include "can/messages/Nissan/CellVoltages.hpp"
#include "can/messages/Nissan/PackTemperatures.hpp"
#include "can/messages/Nissan/BatteryState.hpp"
#include "can/messages/Nissan/BatteryStatus.hpp"
#include "can/messages/SMA/InverterCommand.hpp"
#include "can/messages/SMA/InverterIdentity.hpp"
#include "can/messages/SMA/InverterManufacturer.hpp"
#include "can/messages/Nissan/Ids.hpp"
#include "monitor/Nissan/Monitor.hpp"
#include "inverter/SMA/SunnyBoyStorage.hpp"
#include "can/messages/TSUN/InverterChargeDischargeCmd.hpp"
#include "can/messages/TSUN/InverterInfoRequest.hpp"
#include "can/messages/TSUN/InverterSleepAwakeCmd.hpp"
#include "inverter/TSUN/TSOL-H50K.hpp"

#include "contactor/Contactor.hpp"
#include "core/Timer.hpp"
#include "core/Callback.hpp"

namespace color {
const char* red   = "\x1b[31m";
const char* bright_red   = "\x1b[91m";
const char* green = "\x1b[32m";
const char* bright_green = "\x1b[92m";
const char* blue  = "\x1b[34m";
const char* bright_blue  = "\x1b[94m";
const char* reset = "\x1b[0m";
}
              
gpiod_line* openOutput(gpiod_chip* chip, unsigned pin, const char* name)
{
   gpiod_line* gpio = gpiod_chip_get_line(chip, pin);
   if (gpio == nullptr)
   {
      std::cerr << "failed opening gpiochip pin " << pin << ":" << strerror(errno) << std::endl;
      return nullptr;
   }
   if (gpiod_line_request_output(gpio, name, 0) != 0)
   {
      std::cerr << "failed requesting gpiochip pin " << pin << " as output:" << strerror(errno) << std::endl;
      return nullptr;
   }
   return gpio;
}

class PrintingContactor: public contactor::Contactor
{
public:
   enum State {
      OPEN,
      CLOSING,
      CLOSED
   };
   
   PrintingContactor(int timerfd):
      m_timerfd(timerfd),
      m_gpio_chip(nullptr),
      m_gpio_contactor_neg(nullptr),
      m_gpio_contactor_pos(nullptr),
      m_gpio_led1(nullptr),
      m_safe_to_operate(false),
      m_requested_state(OPEN),
      m_state(OPEN)
   {
      m_gpio_chip = gpiod_chip_open_by_number(0);

      if(m_gpio_chip == nullptr)
      {
         std::cerr << "failed opening gpiochip0:" << strerror(errno) << std::endl;
         return;
      }

      m_gpio_contactor_neg = openOutput(m_gpio_chip, 6, "relay_neg");
      m_gpio_contactor_pos = openOutput(m_gpio_chip, 5, "relay_pos");
      m_gpio_led1 = openOutput(m_gpio_chip, 4, "led1");
   }

   virtual void setSafeToOperate(bool safe)
   {
      std::cout << color::red << ">>> contactor is " << (safe ? "safe" : "unsafe") << " to operate" << color::reset << std::endl;
      m_safe_to_operate = safe;
      update();
   }

   virtual bool isClosed() const
   {
      return m_state == CLOSED;
   }
   
   virtual void open()
   {
      m_requested_state = OPEN;
      update();
   }
   
   virtual void close()
   {
      m_requested_state = CLOSED;
      update();
   }

   void openBoth()
   {
      if (m_gpio_contactor_neg) gpiod_line_set_value(m_gpio_contactor_neg, 1);
      if (m_gpio_contactor_pos) gpiod_line_set_value(m_gpio_contactor_pos, 1);
      if (m_gpio_led1) gpiod_line_set_value(m_gpio_led1, 0);
      
      std::cout << color::bright_red << ">>>> contactor opened" << color::reset << std::endl;
      m_state = OPEN;
   }

   void update()
   {
      if (m_safe_to_operate && m_requested_state == CLOSED)
      {
         if (m_state == OPEN) closeNegative();
      }
      else if (not m_safe_to_operate or m_requested_state == OPEN)
      {
         if (m_state != OPEN) openBoth();
      }
   }
   
   void closeNegative()
   {
      m_state = CLOSING;
      std::cout << color::bright_red << ">>>> contactor closing..." << color::reset << std::endl;
      struct itimerspec its = itimerspec();
      its.it_value.tv_sec = 3;
      timerfd_settime(m_timerfd, 0, &its, NULL);
      
      if (m_gpio_contactor_neg) gpiod_line_set_value(m_gpio_contactor_neg, 0);
      if (m_gpio_led1) gpiod_line_set_value(m_gpio_led1, 1);
   }
   
   void closePositive()
   {
      if (m_gpio_contactor_pos) gpiod_line_set_value(m_gpio_contactor_pos, 0);
      std::cout << color::bright_red <<  ">>>> contactor closed" << color::reset << std::endl;
      m_state = CLOSED;
   }

public:
   int m_timerfd;
   gpiod_chip* m_gpio_chip;
   gpiod_line* m_gpio_contactor_neg;
   gpiod_line* m_gpio_contactor_pos;
   gpiod_line* m_gpio_led1;
   bool m_safe_to_operate;
   State m_requested_state;
   State m_state;
};

class NissanSink: public can::FrameSink
{
public:
   NissanSink(monitor::Nissan::Monitor& monitor): m_monitor(monitor)
   {}

   void* mem()
   {
      return m_message_memory;
   }
   
   const can::messages::Nissan::Message* decode(const can::DataFrame& f)
   {
      using namespace can::messages::Nissan;
      Message* msg;

      msg = new(mem()) BatteryStatus(f);
      if (msg->valid()) return msg;

      msg = new(mem()) CellVoltages(f);
      if (msg->valid()) return msg;

      msg = new(mem()) PackTemperatures(f);
      if (msg->valid()) return msg;

      msg = new(mem()) CellVoltageRange(f);
      if (msg->valid()) return msg;

      msg = new(mem()) BatteryState(f);
      if (msg->valid()) return msg;

      return NULL;
   }
   
   virtual void sink(const can::DataFrame& f)
   {
      const can::messages::Nissan::Message* msg = decode(f);

      if (msg)
      {
         std::cout << color::bright_blue << "<BAT IN> " << *msg << color::reset << std::endl;
         m_monitor.sink(*msg);
      }
   }

   monitor::Nissan::Monitor& m_monitor;
   uint8_t m_message_memory[1024];
};

class SmaSink: public can::FrameSink
{
public:
   SmaSink(inverter::SMA::SunnyBoyStorage& inverter): m_inverter(inverter)
   {
   }

   void* mem()
   {
      return m_message_memory;
   }

   const can::messages::Message* decode(const can::DataFrame& f)
   {
      using namespace can::messages::SMA;
      can::messages::Message* msg;

      msg = new(mem()) InverterCommand(f);
      if (msg->valid()) return msg;

      msg = new(mem()) InverterIdentity(f);
      if (msg->valid()) return msg;

      msg = new(mem()) InverterManufacturer(f);
      if (msg->valid()) return msg;
      
      return NULL;
   }
   
   virtual void sink(const can::DataFrame& f)
   {
      const can::messages::Message* msg = decode(f);
      
      if (msg)
      {
         std::cout << color::bright_green << "<INV IN>  " << *msg << color::reset << std::endl;
         m_inverter.sink(*msg);
      }
   }

   inverter::SMA::SunnyBoyStorage& m_inverter;
   uint8_t m_message_memory[1024];
};

class TsunSink: public can::FrameSink
{
public:
   TsunSink(inverter::TSUN::TSOL_H50K& inverter): m_inverter(inverter)
   {
   }

   void* mem()
   {
      return m_message_memory;
   }

   const can::messages::Message* decode(const can::DataFrame& f)
   {
      using namespace can::messages::TSUN;
      can::messages::Message* msg;

      msg = new(mem()) InverterInfoRequest(f);
      if (msg->valid()) return msg;

      msg = new(mem()) InverterChargeDischargeCmd(f);
      if (msg->valid()) return msg;

      msg = new(mem()) InverterSleepAwakeCmd(f);
      if (msg->valid()) return msg;

      return NULL;
   }

   virtual void sink(const can::DataFrame& f)
   {
      const can::messages::Message* msg = decode(f);

      if (msg)
      {
         std::cout << color::bright_green << "<INV IN>  " << *msg << color::reset << std::endl;
         m_inverter.sink(*msg);
      }
   }

   inverter::TSUN::TSOL_H50K& m_inverter;
   uint8_t m_message_memory[1024];
};

class CanSender: public can::FrameSink
{
public:
   CanSender(int fd, const char* name, const char* color): m_fd(fd), m_name(name), m_color(color)
   {
   }

   virtual void sink(const can::DataFrame& f)
   {
      std::cout << m_color << "<" << m_name << " OUT> " << f << color::reset << std::endl;

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
   const char* m_name;
   const char* m_color;
};

class EpollTimer: public core::Timer
{
public:
   EpollTimer(int epoll_fd):
      m_epoll_fd(epoll_fd)
   {
      m_fd = timerfd_create(CLOCK_MONOTONIC, 0);
   }
   
   virtual void registerPeriodicCallback(core::Invokable* invokable, unsigned period_ms)
   {
      int sec = period_ms / 1000;
      int ms  = period_ms % 1000;
      struct itimerspec its = itimerspec();
      its.it_interval.tv_nsec = ms * 1000000;
      its.it_interval.tv_sec  = sec;
      its.it_value = its.it_interval;
      timerfd_settime(m_fd, 0, &its, NULL);
      
      struct epoll_event ev;
      ev.events = EPOLLIN;
      ev.data.fd = m_fd;
      if (epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, m_fd, &ev) == -1) {
         perror("epoll_ctl: timer");
         exit(EXIT_FAILURE);
      }
      m_invokable = invokable;
   }
   
   virtual void deregisterCallback(core::Invokable*){}

   void expired()
   {
      uint64_t num_expirations;
      (void)read(m_fd, &num_expirations, sizeof(num_expirations));
      m_invokable->invoke();
   }

   int m_epoll_fd;
   int m_fd;
   core::Invokable* m_invokable;
};

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
   int battery_socket, inverter_socket, contactor_timer, poll_timer;

   if (argc != 3)
   {
      fprintf(stderr, "usage: %s <can_interface_battery> <can_interface_inverter>\n", argv[0]);
      return 1;
   }

   battery_socket = openSocket(argv[1]);
   inverter_socket = openSocket(argv[2]);

   const unsigned MAX_EVENTS = 10;
   struct epoll_event ev, events[MAX_EVENTS];
   int nfds, epollfd;

   epollfd = epoll_create1(0);
   if (epollfd == -1) {
      perror("epoll_create1");
      exit(EXIT_FAILURE);
   }

   ev.events = EPOLLIN;
   ev.data.fd = battery_socket;
   if (epoll_ctl(epollfd, EPOLL_CTL_ADD, battery_socket, &ev) == -1) {
      perror("epoll_ctl: can socket");
      exit(EXIT_FAILURE);
   }
   ev.data.fd = inverter_socket;
   if (epoll_ctl(epollfd, EPOLL_CTL_ADD, inverter_socket, &ev) == -1) {
      perror("epoll_ctl: can socket");
      exit(EXIT_FAILURE);
   }

   contactor_timer = timerfd_create(CLOCK_MONOTONIC, 0);
   if (contactor_timer == -1)
   {
      perror("timerfd_create");
      exit(EXIT_FAILURE);
   }

   ev.events = EPOLLIN;
   ev.data.fd = contactor_timer;
   if (epoll_ctl(epollfd, EPOLL_CTL_ADD, contactor_timer, &ev) == -1) {
      perror("epoll_ctl: contactor_timer");
      exit(EXIT_FAILURE);
   }

   poll_timer = timerfd_create(CLOCK_MONOTONIC, 0);
   if (poll_timer == -1)
   {
      perror("timerfd_create");
      exit(EXIT_FAILURE);
   }
   struct itimerspec its = itimerspec();
   its.it_interval.tv_nsec = 0;
   its.it_interval.tv_sec = 1;
   its.it_value = its.it_interval;
   timerfd_settime(poll_timer, 0, &its, NULL);

   ev.events = EPOLLIN;
   ev.data.fd = poll_timer;
   if (epoll_ctl(epollfd, EPOLL_CTL_ADD, poll_timer, &ev) == -1) {
      perror("epoll_ctl: poll_timer");
      exit(EXIT_FAILURE);
   }
   
   PrintingContactor contactor(contactor_timer);
   monitor::Nissan::Monitor monitor(contactor);
   NissanSink battery_sink(monitor);
   can::services::Nissan::FrameAggregator aggregator(battery_sink);
   CanSender battery_sender(battery_socket, "BAT", color::blue);
   can::services::Nissan::GroupPoller poller(battery_sender);

   CanSender inverter_sender(inverter_socket, "INV", color::green);
   EpollTimer inverter_timer(epollfd);
   inverter::SMA::SunnyBoyStorage inverter(inverter_sender, inverter_timer, monitor, contactor);
   SmaSink inverter_sink(inverter);
   
   while (1)
   {
      nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
      if (nfds == -1) {
          perror("epoll_wait");
          exit(EXIT_FAILURE);
      }

      for (int n = 0; n < nfds; ++n)
      {
         if (events[n].data.fd == battery_socket ||
             events[n].data.fd == inverter_socket)
         {
            struct can_frame frame;
            int nbytes = read(events[n].data.fd, &frame, sizeof(struct can_frame));

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

            if (events[n].data.fd == battery_socket)
            {            
               poller.received(f);
               aggregator.sink(f);
            }
            else
            {
               inverter_sink.sink(f);
            }
         }
         else if (events[n].data.fd == inverter_timer.m_fd)
         {
            inverter_timer.expired();
         }
         else if (events[n].data.fd == poll_timer)
         {
            poller.poll();
            uint64_t num_expirations;
            (void)read(poll_timer, &num_expirations, sizeof(num_expirations));
         }
         else if (events[n].data.fd == contactor_timer)
         {
            contactor.closePositive();
            uint64_t num_expirations;
            (void)read(contactor_timer, &num_expirations, sizeof(num_expirations));
         }
      }
   }

   return 0;
}
