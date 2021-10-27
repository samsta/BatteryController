#include <gmock/gmock.h>
#include "can/messages/Nissan/BatteryPowerLimits.hpp"
#include "can/StandardDataFrame.hpp"
#include <vector>

using namespace can::messages::Nissan;
using namespace testing;

TEST(BatteryPowerLimits, valid)
{
   EXPECT_TRUE(BatteryPowerLimits(can::StandardDataFrame("1DC#0000000000000000")).valid());
}

TEST(BatteryPowerLimits, invalidId)
{
   EXPECT_FALSE(BatteryPowerLimits(can::StandardDataFrame("1FF#0000000000000000")).valid());
}

TEST(BatteryPowerLimits, invalidLength)
{
   EXPECT_FALSE(BatteryPowerLimits(can::StandardDataFrame("1DC#00000000000000")).valid());
}

TEST(BatteryPowerLimits, getDischargeLimit)
{
   can::StandardDataFrame frame("1dc#6E0C8FFD0540C696");

   BatteryPowerLimits battery(frame);

   EXPECT_FLOAT_EQ(110.0, battery.getDischargePowerLimit());
}

TEST(BatteryPowerLimits, getChargeLimit)
{
   can::StandardDataFrame frame("1dc#6E0C7FFD0540C696");

   BatteryPowerLimits battery(frame);

   EXPECT_FLOAT_EQ(49.75, battery.getChargePowerLimit());
}

TEST(BatteryPowerLimits, invalidDischargeLimit)
{
   EXPECT_FALSE(BatteryPowerLimits(can::StandardDataFrame("1DC#FF0C8FFD0540C696")).valid());
}
