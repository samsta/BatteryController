#include "StandardDataFrame.hpp"
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include "logging/Hex.hpp"

namespace can {

StandardDataFrame::StandardDataFrame(): DataFrame(), m_data(){}

StandardDataFrame::StandardDataFrame(const char* str): DataFrame(), m_data()
{
   if (not str) return;

   uint32_t id;
   char hash;
   char dummy;
   int num = sscanf(str, "%" SCNx32 "%c%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%c",
         &id, &hash, &m_data[0], &m_data[1], &m_data[2], &m_data[3], &m_data[4], &m_data[5], &m_data[6], &m_data[7], &dummy);

   if (num > 10) return;
   if (num < 2) return;
   if (hash != '#') return;

   setId(id);
   setSize(num - 2);
}

StandardDataFrame::StandardDataFrame(uint32_t id, const uint8_t data[], unsigned size):
      DataFrame(id, size > sizeof(m_data) ? sizeof(m_data) : size),
      m_data()
{
   if (data == NULL)
   {
      setSize(0);
      return;
   }
   memcpy(m_data, data, size);
}

const uint8_t* StandardDataFrame::data() const
{
   return m_data;
}

}
