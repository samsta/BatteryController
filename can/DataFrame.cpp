#include "DataFrame.hpp"
#include <string.h>

namespace can {
namespace {

const unsigned BITS_PER_BYTE = 8;

}

DataFrame::DataFrame(): id(), data(){}

DataFrame::DataFrame(uint16_t id, const uint8_t data[], unsigned length)
{
   memcpy(this->data, data, length);
}
   
uint64_t DataFrame::getBitField(unsigned start_bit, unsigned num_bits) const
{
   const uint8_t* start_byte = &data[start_bit / BITS_PER_BYTE];
   const uint8_t* end_byte   = &data[(start_bit + num_bits - 1) / BITS_PER_BYTE];

   uint64_t v = (*start_byte) >> (start_bit % BITS_PER_BYTE);
   unsigned shift = 8 - (start_bit % BITS_PER_BYTE);
   while (end_byte != start_byte++)
   {
      v += uint64_t(*start_byte) << shift;
      shift += BITS_PER_BYTE;
   }

   if (num_bits == 64) return v;
   return v & ((1ULL << num_bits) - 1);
}

int64_t DataFrame::getSignedBitField(unsigned start_bit, unsigned num_bits) const
{
   uint64_t v = getBitField(start_bit, num_bits);
   uint64_t sign_bit = 1ULL << (num_bits - 1);
   return (v & (sign_bit - 1)) - (v & sign_bit);
}

   
}
