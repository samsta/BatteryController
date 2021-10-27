#include <gmock/gmock.h>
#include "can/messages/Nissan/BatteryVoltsCurrent.hpp"
#include "can/StandardDataFrame.hpp"
#include <vector>

using namespace can::messages::Nissan;
using namespace testing;

TEST(BatteryVoltsCurrent, invalidVoltage)
{
   EXPECT_FALSE(BatteryVoltsCurrent(can::StandardDataFrame("1DB#FFE0FFC67F000087")).valid());
}

TEST(BatteryVoltsCurrent, valid)
{
   EXPECT_TRUE(BatteryVoltsCurrent(can::StandardDataFrame("1DB#0000000000000000")).valid());
}

TEST(BatteryVoltsCurrent, invalidId)
{
   EXPECT_FALSE(BatteryVoltsCurrent(can::StandardDataFrame("1FF#0000000000000000")).valid());
}

TEST(BatteryVoltsCurrent, invalidLength)
{
   EXPECT_FALSE(BatteryVoltsCurrent(can::StandardDataFrame("1DB#00000000000000")).valid());
}

TEST(BatteryVoltsCurrent, getVoltage1)
{
   can::StandardDataFrame frame("1DB#0020B6AF7F000087");

   BatteryVoltsCurrent battery(frame);

   EXPECT_FLOAT_EQ(365.0, battery.getTotalVoltage());
}

TEST(BatteryVoltsCurrent, getVoltage2)
{
   can::StandardDataFrame frame("1DB#0020C8807F000087");

   BatteryVoltsCurrent battery(frame);

   EXPECT_FLOAT_EQ(401.0, battery.getTotalVoltage());
}

TEST(BatteryVoltsCurrent, getCurrent1)
{
   can::StandardDataFrame frame("1db#00207FFD0540C696");

   BatteryVoltsCurrent battery(frame);

   EXPECT_FLOAT_EQ(0.5, battery.getTotalCurrent());
}

//TEST(BatteryVoltsCurrent, getCurrent2)
//{
//   can::StandardDataFrame frame("1db#FFC0FFFD0540C696");
//
//   BatteryVoltsCurrent battery(frame);
//
//   EXPECT_FLOAT_EQ(-1.0, battery.getTotalCurrent());
//}
