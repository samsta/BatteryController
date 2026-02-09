/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "InputPin.hpp"
#include <errno.h>
#include <string.h>
#include <iostream>

namespace core {
namespace libgpiod {

InputPin::InputPin(
      unsigned chip_number,
      unsigned pin_number,
      const char* name):
   m_name(name),
   m_gpio(nullptr)
{
   gpiod_chip* gpio_chip = gpiod_chip_open_by_number(chip_number);
   if(gpio_chip == nullptr)
   {
      std::cerr << "ERROR: InputPin " << m_name <<
            ": failed opening gpiochip" << chip_number << ": " << strerror(errno) << std::endl;
      return;
   }

   gpiod_line* gpio = gpiod_chip_get_line(gpio_chip, pin_number);
   if (gpio == nullptr)
   {
      std::cerr << "ERROR: InputPin " << m_name <<
            ": failed opening gpiochip pin " << gpio_chip << ": " << strerror(errno) << std::endl;
      return;
   }

   if (gpiod_line_request_input(gpio, m_name.c_str()) != 0)
   {
      std::cerr << "ERROR: InputPin " << m_name <<
            ": failed requesting gpiochip pin " << gpio_chip << " as input: " << strerror(errno) << std::endl;
      return;
   }

   m_gpio = gpio;
}

bool InputPin::get()
{
   if (!isOpen())
   {
      std::cerr << "ERROR: InputPin " << m_name
                << ": failed getting state as it is not open" << std::endl;
      return false;
   }

   int v = gpiod_line_get_value(m_gpio);
   if (v < 0)
   {
      std::cerr << "ERROR: InputPin " << m_name
                << ": failed reading value: " << strerror(errno) << std::endl;
      return false;
   }
   return (v != 0);
}

bool InputPin::isOpen() const
{
   return m_gpio != nullptr;
}


}
}
