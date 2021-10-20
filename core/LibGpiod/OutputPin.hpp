/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CORE_LIBGPIOD_OUTPUTPIN_HPP
#define _CORE_LIBGPIOD_OUTPUTPIN_HPP

#include "core/OutputPin.hpp"
#include <gpiod.h>
#include <string>

namespace core {
namespace libgpiod {

class OutputPin: public ::core::OutputPin
{
public:
   OutputPin(unsigned chip_number,
             unsigned pin_number,
             const char* name,
             State initial_state = LOW);

   virtual void set(State state);
   bool isOpen() const;

private:
   const std::string m_name;
   gpiod_line* m_gpio;
};

}
}

#endif // _CORE_LIBGPIOD_OUTPUTPIN_HPP
