/* SPDX-License-Identifier: GPL-3.0-or-later */

#include "DataFrame.hpp"
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include "logging/Hex.hpp"

#ifdef HAS_STD_STRING
#include <sstream>
#endif

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

void DataFrame::setByte(unsigned byte_index, uint8_t value)
{
   data()[byte_index] = value;
}

uint16_t DataFrame::getUnsignedShort(unsigned start_byte) const
{
   return getUnsignedLong(start_byte, 2);
}

int16_t DataFrame::getSignedShort(unsigned start_byte) const
{
   return getUnsignedShort(start_byte);
}

void DataFrame::setUnsignedShort(unsigned start_byte, uint16_t value)
{
   data()[start_byte++] = value >> 8;
   data()[start_byte]   = value;
}
   
uint32_t DataFrame::getUnsignedLong(unsigned start_byte, unsigned num_bytes) const
{
   uint32_t v = 0;
   while (num_bytes--)
   {
      v = (v<<8) + uint32_t(data()[start_byte++]);
   }
   return v;
}

void DataFrame::setUnsignedLong(unsigned start_byte, uint32_t value)
{
   data()[start_byte++] = value >> 24;
   data()[start_byte++] = value >> 16;
   data()[start_byte++] = value >> 8;
   data()[start_byte]   = value;
}

#ifdef HAS_STD_STRING
std::string DataFrame::str() const
{
   std::stringstream ss;
   ss << *this;
   return ss.str();
}
#endif
   
logging::ostream& operator<<(logging::ostream& os, const DataFrame& frame)
{
   os << logging::Hex(frame.id()) << "#";
   for (unsigned k = 0; k < frame.size(); k++)
   {
      os << logging::HexByte(frame.data()[k]);
   }
   return os;
}

bool DataFrame::operator==(const DataFrame& other) const
{
   if (other.id() != id()) return false;
   if (other.size() != size()) return false;
   return memcmp(data(), other.data(), size()) == 0;
}

bool DataFrame::operator!=(const DataFrame& other) const
{
   return not operator==(other);
}

}
