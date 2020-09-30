/* SPDX-License-Identifier: GPL-3.0-or-later */

#ifndef _CAN_MESSAGES_SMA_STRING_HPP
#define _CAN_MESSAGES_SMA_STRING_HPP

#include "can/StandardDataFrame.hpp"
#include "can/messages/SMA/Ids.hpp"

namespace can {
namespace messages {
namespace SMA {

class String: public StandardDataFrame
{
public:
   String(Ids id);
   String(Ids id, uint8_t index, const char* string);
};

}
}
}

#endif // _CAN_MESSAGES_SMA_STRING_HPP
