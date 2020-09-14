#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/Nissan/CellVoltages.hpp"
#include <vector>

using namespace can::messages::Nissan;
using namespace testing;

namespace {

class DataFrame7bbGroup2: public can::DataFrame
{
public:
   DataFrame7bbGroup2(unsigned id = 0x7bb, unsigned size = 198): DataFrame(id, size), m_data(size)
   {
      // group is at offset 1
      m_data[1] = 0x02;
   }

   virtual const uint8_t* data() const
   {
      return m_data.data();
   }

   std::vector<uint8_t> m_data;
};

}

TEST(CellVoltages, valid)
{
   DataFrame7bbGroup2 frame;

   EXPECT_TRUE(CellVoltages(frame).valid());
}

TEST(CellVoltages, invalidIfNot7bb)
{
   DataFrame7bbGroup2 frame(0x7cc);

   EXPECT_FALSE(CellVoltages(frame).valid());
}

TEST(CellVoltages, invalidIfWrongSize)
{
   DataFrame7bbGroup2 frame(0x7bb, 8);

   EXPECT_FALSE(CellVoltages(frame).valid());
}

TEST(CellVoltages, invalidIfNotGroup2)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[1]++;

   EXPECT_FALSE(CellVoltages(frame).valid());
}

TEST(CellVoltages, readAllZeroIfAllBytesZero)
{
   DataFrame7bbGroup2 frame;
   CellVoltages voltages(frame);

   for(unsigned k = 0; k < 96; k ++)
   {
      SCOPED_TRACE(testing::Message("cell number ") << k);
      EXPECT_EQ(0, voltages.getVoltage(k));
   }
}

TEST(CellVoltages, readNaNForCellsBeyondBounds)
{
   DataFrame7bbGroup2 frame;
   CellVoltages voltages(frame);

   EXPECT_TRUE(isnan(voltages.getVoltage(96)));
}

TEST(CellVoltages, unitIs0v001)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[2] = 0x00;
   frame.m_data[3] = 0x01;
   CellVoltages voltages(frame);

   EXPECT_FLOAT_EQ(0.001, voltages.getVoltage(0));
}

TEST(CellVoltages, maxIs65v535)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[2] = 0xff;
   frame.m_data[3] = 0xff;
   CellVoltages voltages(frame);

   EXPECT_FLOAT_EQ(65.535, voltages.getVoltage(0));
}

TEST(CellVoltages, adjacentValues)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[2] = 0x00;
   frame.m_data[3] = 0x00;
   frame.m_data[4] = 0xff;
   frame.m_data[5] = 0xff;
   frame.m_data[6] = 0x80;
   frame.m_data[7] = 0x00;
   CellVoltages voltages(frame);

   EXPECT_FLOAT_EQ( 0.000, voltages.getVoltage(0));
   EXPECT_FLOAT_EQ(65.535, voltages.getVoltage(1));
   EXPECT_FLOAT_EQ(32.768, voltages.getVoltage(2));
}

TEST(CellVoltages, lastCell)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[192] = 0x0A;
   frame.m_data[193] = 0x01;
   CellVoltages voltages(frame);

   EXPECT_FLOAT_EQ( 2.561, voltages.getVoltage(95));
}

TEST(CellVoltages, toString)
{
   std::ostringstream string;
   DataFrame7bbGroup2 frame;
   frame.m_data[2] = 0x0f;
   frame.m_data[3] = 0x00;
   frame.m_data[192] = 0x0A;
   frame.m_data[193] = 0x01;
   CellVoltages voltages(frame);
   string << voltages;

   EXPECT_THAT(string.str(), StartsWith("CellVoltages: 3.84V"));
   EXPECT_THAT(string.str(), EndsWith(" 2.561V"));
   // Expect 95 voltages directly followed by a comma and space each, and another voltage without comma.
   // The voltages may contain a decimal point followed by decimal places
   EXPECT_THAT(string.str(), MatchesRegex("CellVoltages: ([0-9]+(.[0-9]+)?V, ){95}[0-9]+(.[0-9]+)?V"));
}

TEST(CellVoltages, invalidToString)
{
   std::ostringstream string;
   CellVoltages voltages(DataFrame7bbGroup2(0));
   string << voltages;
   EXPECT_EQ(string.str(), "CellVoltages: invalid");
}

