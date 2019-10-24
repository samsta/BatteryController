#include "DataFrame.hpp"
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include "logging/Hex.hpp"

namespace can {
namespace {

const unsigned BITS_PER_BYTE = 8;
}

DataFrame::DataFrame(): m_id(INVALID_ID), m_size(0){}

DataFrame::DataFrame(int32_t id, unsigned size): m_id(id), m_size(size){}

void DataFrame::setSize(unsigned size)
{
   m_size = size;
}

unsigned DataFrame::size() const
{
   return m_size;
}

void DataFrame::setId(int32_t id)
{
   m_id = id;
}

uint32_t DataFrame::id() const
{
   return m_id;
}

bool DataFrame::valid() const
{
   return m_id != INVALID_ID;
}

DataFrame::const_iterator DataFrame::begin() const
{
   return data();
}

DataFrame::const_iterator DataFrame::end() const
{
   return data() + size();
}

uint64_t DataFrame::getBitField(unsigned start_bit, unsigned num_bits) const
{
   const uint8_t* start_byte = &data()[start_bit / BITS_PER_BYTE];
   const uint8_t* end_byte   = &data()[(start_bit + num_bits - 1) / BITS_PER_BYTE];

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

uint16_t DataFrame::getUnsignedShort(unsigned start_byte) const
{
   return (uint16_t(data()[start_byte]) << 8) + data()[start_byte + 1];
}


logging::ostream& operator<<(logging::ostream& os, const DataFrame& frame)
{
   os << logging::Hex(frame.id()) << "#";
   for (unsigned k = 0; k < frame.size(); k++)
   {
      os << logging::HexByte(frame.data()[k]);
   }
   return os;
}


}
