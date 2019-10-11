#ifndef _CAN_DATAFRAME_HPP
#define _CAN_DATAFRAME_HPP

#include <stdint.h>

namespace can {

class DataFrame
{
public:
   DataFrame();
   DataFrame(const char*);
   DataFrame(uint32_t id, const uint8_t data[], unsigned size = 8);

   bool valid() const;
   unsigned size() const;
   const uint8_t* data() const;
   uint32_t id() const;
   uint64_t getBitField(unsigned start_bit, unsigned num_bits) const;
   int64_t  getSignedBitField(unsigned start_bit, unsigned num_bits) const;

private:
   int32_t m_id;
   unsigned m_size;
   uint8_t m_data[8];
};

}

#endif // _CAN_DATAFRAME_HPP
