#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/Nissan/CellVoltageRange.hpp"
#include <vector>

using namespace can::messages::Nissan;
using namespace testing;

namespace {

class DataFrame7bbGroup2: public can::DataFrame
{
public:
   DataFrame7bbGroup2(unsigned id = 0x7bb, unsigned size = 28): DataFrame(id, size), m_data(size)
   {
      // group is at offset 1
      m_data[1] = 0x03;
   }

   virtual const uint8_t* data() const
   {
      return m_data.data();
   }

   virtual uint8_t* data()
   {
      return m_data.data();
   }

   std::vector<uint8_t> m_data;
};

}

TEST(CellVoltageRange, valid)
{
   DataFrame7bbGroup2 frame;

   EXPECT_TRUE(CellVoltageRange(frame).valid());
}

TEST(CellVoltageRange, invalidIfNot7bb)
{
   DataFrame7bbGroup2 frame(0x7cc);

   EXPECT_FALSE(CellVoltageRange(frame).valid());
}

TEST(CellVoltageRange, invalidIfSizeTooSmall)
{
   DataFrame7bbGroup2 frame(0x7bb, 27);

   EXPECT_FALSE(CellVoltageRange(frame).valid());
}

TEST(CellVoltageRange, invalidIfNotGroup3)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[1]++;

   EXPECT_FALSE(CellVoltageRange(frame).valid());
}

TEST(CellVoltageRange, readAllZeroIfAllBytesZero)
{
   DataFrame7bbGroup2 frame;
   CellVoltageRange state(frame);

   EXPECT_EQ(0, state.getMax());
   EXPECT_EQ(0, state.getMin());
}

TEST(CellVoltageRange, maxVoltageUnitIs0V001)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[13] = 0x01;
   CellVoltageRange state(frame);

   EXPECT_FLOAT_EQ(0.001, state.getMax());
}

TEST(CellVoltageRange, minVoltageUnitIs0V001)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[15] = 0x01;
   CellVoltageRange state(frame);

   EXPECT_FLOAT_EQ(0.001, state.getMin());
}

TEST(CellVoltageRange, maxVoltageMaximumIs65V535)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[12] = 0xff;
   frame.m_data[13] = 0xff;
   CellVoltageRange state(frame);

   EXPECT_FLOAT_EQ(65.535, state.getMax());
}

TEST(CellVoltageRange, minVoltageMaximumIs65V535)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[14] = 0xff;
   frame.m_data[15] = 0xff;
   CellVoltageRange state(frame);

   EXPECT_FLOAT_EQ(65.535, state.getMin());
}

TEST(CellVoltageRange, toString)
{
   std::ostringstream string;
   DataFrame7bbGroup2 frame;
   // max
   frame.m_data[12] = 0x10;
   frame.m_data[13] = 0x01;
   // min
   frame.m_data[14] = 0x0d;
   frame.m_data[15] = 0xff;

   CellVoltageRange state(frame);
   string << state;

   EXPECT_EQ(string.str(), "CellVoltageRange: 7BB min=3.583V max=4.097V");
}

TEST(CellVoltageRange, invalidToString)
{
   std::ostringstream string;
   CellVoltageRange state(DataFrame7bbGroup2(0));
   string << state;
   EXPECT_EQ(string.str(), "CellVoltageRange: 7BB invalid");
}


