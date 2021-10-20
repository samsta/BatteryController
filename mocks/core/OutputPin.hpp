/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _MOCKS_CORE_OUTPUTPIN_HPP
#define _MOCKS_CORE_OUTPUTPIN_HPP

#include "core/OutputPin.hpp"

namespace mocks {
namespace core {

class OutputPin: public ::core::OutputPin
{
public:
   MOCK_METHOD(void, set, (State state));
};

}
}

#endif // _MOCKS_CORE_OUTPUTPIN_HPP
