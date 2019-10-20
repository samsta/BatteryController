#include "LargeDataFrame.hpp"
#include <string.h>

namespace can {

LargeDataFrame::LargeDataFrame(uint32_t id, unsigned size):
   DataFrame(id, size),
   m_data(new uint8_t[size]),
   m_cur_size(0)
{
}

LargeDataFrame::LargeDataFrame(uint32_t id, const uint8_t* data, unsigned size):
   DataFrame(id, size),
   m_data(new uint8_t[size]),
   m_cur_size(0)
{
   append(data, size);
}

void LargeDataFrame::append(const uint8_t* data, unsigned size)
{
   if (size > this->size() - m_cur_size) size = this->size() - m_cur_size;

   memcpy(m_data + m_cur_size, data, size);
   m_cur_size += size;
}


LargeDataFrame::~LargeDataFrame()
{
   delete [] m_data;
}

const uint8_t* LargeDataFrame::data() const
{
   return m_data;
}

bool LargeDataFrame::valid() const
{
   return m_cur_size == size();
}

}
