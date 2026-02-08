/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CORE_LIBGPIOD_INPUTPIN_HPP
#define _CORE_LIBGPIOD_INPUTPIN_HPP

#include "core/InputPin.hpp"
#include <gpiod.h>
#include <string>

namespace core {
namespace libgpiod {

class InputPin: public ::core::InputPin
{
public:
   InputPin(unsigned chip_number,
             unsigned pin_number,
             const char* name);

   virtual bool get();
   bool isOpen() const;

private:
   const std::string m_name;
   gpiod_line* m_gpio;
   bool input_state;
};

}
}

#endif // _CORE_LIBGPIOD_INPUTPIN_HPP
