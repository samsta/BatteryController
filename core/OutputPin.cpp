/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "core/OutputPin.hpp"

namespace core {

void NullOutputPin::set(State state)
{
   (void)state;
}


}
