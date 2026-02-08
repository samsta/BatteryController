/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "core/InputPin.hpp"

namespace core {

bool NullInputPin::get()
{
   return input_state;
}


}
