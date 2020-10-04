/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _MOCKS_CAN_FRAMESINK_HPP
#define _MOCKS_CAN_FRAMESINK_HPP

#include "can/FrameSink.hpp"
#include "can/DataFrame.hpp"

namespace mocks {
namespace can {

class FrameSink: public ::can::FrameSink
{
public:
   MOCK_METHOD(void, sink, (const ::can::DataFrame&));
};

}
}

#endif // _MOCKS_CAN_FRAMESINK_HPP
