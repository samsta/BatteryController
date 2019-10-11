#include "DataFrame.hpp"
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

namespace can {
namespace {

const unsigned BITS_PER_BYTE = 8;
const int32_t INVALID_ID = -1;
}

DataFrame::DataFrame(): m_id(INVALID_ID), m_size(0), m_data(){}

DataFrame::DataFrame(const char* str): m_id(INVALID_ID), m_size(0), m_data()
{
   if (not str) return;

   uint32_t id;
   char hash;
   int num = sscanf(str, "%" SCNx32 "%c%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx",
         &id, &hash, &m_data[0], &m_data[1], &m_data[2], &m_data[3], &m_data[4], &m_data[5], &m_data[6], &m_data[7]);
   if (num >= 2 && hash == '#')
   {
      m_id = id;
      m_size = num - 2;
   }
}

DataFrame::DataFrame(uint32_t id, const uint8_t data[], unsigned size):
      m_id(id),
      m_size(size > sizeof(m_data) ? sizeof(m_data) : size),
      m_data()
{
   if (data == NULL)
   {
      m_size = 0;
      return;
   }
   memcpy(m_data, data, size);
}

unsigned DataFrame::size() const
{
   return m_size;
}

uint32_t DataFrame::id() const
{
   return m_id;
}

const uint8_t* DataFrame::data() const
{
   return m_data;
}

bool DataFrame::valid() const
{
   return m_id != INVALID_ID;
}

uint64_t DataFrame::getBitField(unsigned start_bit, unsigned num_bits) const
{
   const uint8_t* start_byte = &m_data[start_bit / BITS_PER_BYTE];
   const uint8_t* end_byte   = &m_data[(start_bit + num_bits - 1) / BITS_PER_BYTE];

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
