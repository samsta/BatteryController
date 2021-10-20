/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "OutputPin.hpp"
#include <errno.h>
#include <string.h>
#include <iostream>

namespace core {
namespace libgpiod {

OutputPin::OutputPin(
      unsigned chip_number,
      unsigned pin_number,
      const char* name,
      State initial_state):
   m_name(name),
   m_gpio(nullptr)
{
   gpiod_chip* gpio_chip = gpiod_chip_open_by_number(chip_number);
   if(gpio_chip == nullptr)
   {
      std::cerr << "ERROR: OutputPin " << m_name <<
            ": failed opening gpiochip" << chip_number << ": " << strerror(errno) << std::endl;
      return;
   }

   gpiod_line* gpio = gpiod_chip_get_line(gpio_chip, pin_number);
   if (gpio == nullptr)
   {
      std::cerr << "ERROR: OutputPin " << m_name <<
            ": failed opening gpiochip pin " << gpio_chip << ": " << strerror(errno) << std::endl;
      return;
   }

   if (gpiod_line_request_output(gpio,
                                 m_name.c_str(),
                                 initial_state == HIGH ? 1 : 0)
       != 0)
   {
      std::cerr << "ERROR: OutputPin " << m_name <<
            ": failed requesting gpiochip pin " << gpio_chip << " as output: " << strerror(errno) << std::endl;
      return;
   }

   m_gpio = gpio;
}

void OutputPin::set(State state)
{
   if (not isOpen())
   {
      std::cerr << "ERROR: OutputPin " << m_name <<
            ": failed setting pin " << (state == HIGH ? "high":"low") << " as it is not open" << std::endl;
      return;
   }

   if (gpiod_line_set_value(m_gpio, state == HIGH ? 1 : 0) != 0)
   {
      std::cerr << "ERROR: OutputPin " << m_name <<
            ": failed setting pin " << (state == HIGH ? "high":"low") << ": " << strerror(errno) << std::endl;
   }
}

bool OutputPin::isOpen() const
{
   return m_gpio != nullptr;
}


}
}
