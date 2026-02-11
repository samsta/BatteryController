/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _MOCKS_CORE_INPUTPIN_HPP
#define _MOCKS_CORE_INPUTPIN_HPP

#include "core/InputPin.hpp"

namespace mocks {
namespace core {

class InputPin: public ::core::InputPin
{
public:
   MOCK_METHOD(bool, get, ());
};

}
}

#endif // _MOCKS_CORE_INPUTPIN_HPP
