#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/Nissan/BatteryState.hpp"
#include <vector>

using namespace can::messages::Nissan;
using namespace testing;

namespace {

class DataFrame7bbGroup2: public can::DataFrame
{
public:
   DataFrame7bbGroup2(unsigned id = 0x7bb, unsigned size = 41): DataFrame(id, size), m_data(size)
   {
      // group is at offset 1
      m_data[1] = 0x01;
   }

   virtual const uint8_t* data() const
   {
      return m_data.data();
   }

   std::vector<uint8_t> m_data;
};

}

TEST(BatteryState, valid)
{
   DataFrame7bbGroup2 frame;

   EXPECT_TRUE(BatteryState(frame).valid());
}

TEST(BatteryState, invalidIfNot7bb)
{
   DataFrame7bbGroup2 frame(0x7cc);

   EXPECT_FALSE(BatteryState(frame).valid());
}

TEST(BatteryState, invalidIfSizeTooSmall)
{
   DataFrame7bbGroup2 frame(0x7bb, 40);

   EXPECT_FALSE(BatteryState(frame).valid());
}

TEST(BatteryState, invalidIfNotGroup2)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[1]++;

   EXPECT_FALSE(BatteryState(frame).valid());
}

TEST(BatteryState, readAllZeroIfAllBytesZero)
{
   DataFrame7bbGroup2 frame;
   BatteryState state(frame);

   EXPECT_EQ(0, state.getPackVoltage());
   EXPECT_EQ(0, state.getHealthPercent());
   EXPECT_EQ(0, state.getSocPercent());
   EXPECT_EQ(0, state.getCapacityAh());
}

TEST(BatteryState, packVoltageUnitIs0V01)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[21] = 0x01;
   BatteryState state(frame);

   EXPECT_FLOAT_EQ(0.01, state.getPackVoltage());
}

TEST(BatteryState, packVoltageMaximumIs655V35)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[20] = 0xff;
   frame.m_data[21] = 0xff;
   BatteryState state(frame);

   EXPECT_FLOAT_EQ(655.35, state.getPackVoltage());
}

TEST(BatteryState, healthUnitIs0Pc01)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[29] = 0x01;
   BatteryState state(frame);

   EXPECT_FLOAT_EQ(0.01, state.getHealthPercent());
}

TEST(BatteryState, healthCanRepresent100Pc)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[28] = 0x27;
   frame.m_data[29] = 0x10;
   BatteryState state(frame);

   EXPECT_FLOAT_EQ(100, state.getHealthPercent());
}

TEST(BatteryState, capacityUnitIs0Ah0001)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[37] = 0x01;
   BatteryState state(frame);

   EXPECT_FLOAT_EQ(0.0001, state.getCapacityAh());
}

TEST(BatteryState, maxCapacityIs1677Ah7216)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[35] = 0xff;
   frame.m_data[36] = 0xff;
   frame.m_data[37] = 0xff;
   BatteryState state(frame);

   EXPECT_FLOAT_EQ(1677.7216, state.getCapacityAh());
}

TEST(BatteryState, socUnitIs0Pc0001)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[33] = 0x01;
   BatteryState state(frame);

   EXPECT_FLOAT_EQ(0.0001, state.getSocPercent());
}

TEST(BatteryState, socCanRepresent100pc)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[31] = 0x0f;
   frame.m_data[32] = 0x42;
   frame.m_data[33] = 0x40;
   BatteryState state(frame);

   EXPECT_FLOAT_EQ(100, state.getSocPercent());
}

TEST(BatteryState, toString)
{
   std::ostringstream string;
   DataFrame7bbGroup2 frame;
   // voltage
   frame.m_data[20] = 0x32;
   frame.m_data[21] = 0x10;
   // health
   frame.m_data[28] = 0x12;
   frame.m_data[29] = 0x34;
   // capacity
   frame.m_data[35] = 0x12;
   frame.m_data[36] = 0x34;
   frame.m_data[37] = 0x56;
   // soc
   frame.m_data[31] = 0x01;
   frame.m_data[32] = 0x23;
   frame.m_data[33] = 0x45;

   BatteryState state(frame);
   string << state;

   EXPECT_EQ(string.str(), "BatteryState: Voltage=128.16V Health=46.6% SOC=7.4565% Capacity=119.305Ah");
}

TEST(BatteryState, invalidToString)
{
   std::ostringstream string;
   BatteryState state(DataFrame7bbGroup2(0));
   string << state;
   EXPECT_EQ(string.str(), "BatteryState: invalid");
}


