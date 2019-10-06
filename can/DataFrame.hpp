#ifndef _CAN_DATAFRAME_HPP
#define _CAN_DATAFRAME_HPP

#include <stdint.h>
#include <array>

namespace can {

class DataFrame
{
public:
   DataFrame();
   DataFrame(uint16_t id, const std::array<uint8_t, 8>& data);
   
   uint16_t id;
   uint8_t data[8];

   uint64_t getBitField(unsigned start_bit, unsigned num_bits) const;
   int64_t  getSignedBitField(unsigned start_bit, unsigned num_bits) const;
};
   
}

#endif // _CAN_DATAFRAME_HPP
