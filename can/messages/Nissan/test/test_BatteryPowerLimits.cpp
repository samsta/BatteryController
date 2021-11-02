#include <gmock/gmock.h>
#include "can/StandardDataFrame.hpp"
#include "can/messages/Nissan/BatteryPowerLimits.hpp"

using namespace can::messages::Nissan;
using namespace testing;

TEST(NissanBatteryPowerLimits, invalidDischargeLimit)
{
   EXPECT_FALSE(BatteryPowerLimits(can::StandardDataFrame("1DC#FFF08FFD0540C696")).valid());
}

TEST(NissanBatteryPowerLimits, invalidChargeLimit)
{
   EXPECT_FALSE(BatteryPowerLimits(can::StandardDataFrame("1DC#00FFFFFD0540C696")).valid());
}

TEST(NissanBatteryPowerLimits, valid)
{
   EXPECT_TRUE(BatteryPowerLimits(can::StandardDataFrame("1DC#0000000000000000")).valid());
}

TEST(NissanBatteryPowerLimits, invalidId)
{
   EXPECT_FALSE(BatteryPowerLimits(can::StandardDataFrame("1FF#0000000000000000")).valid());
}

TEST(NissanBatteryPowerLimits, invalidLength)
{
   EXPECT_FALSE(BatteryPowerLimits(can::StandardDataFrame("1DC#00000000000000")).valid());
}

TEST(NissanBatteryPowerLimits, getDischargeLimit1)
{
   BatteryPowerLimits battery(can::StandardDataFrame("1dc#6E0C8FFD0540C696"));

   EXPECT_FLOAT_EQ(110.0, battery.getDischargePowerLimit_kW());
}

TEST(NissanBatteryPowerLimits, getDischargeLimit2)
{
   can::StandardDataFrame frame("1dc#583C8FFD0540C696");
   BatteryPowerLimits battery(frame);

   EXPECT_FLOAT_EQ(88.0, battery.getDischargePowerLimit_kW());
}

TEST(NissanBatteryPowerLimits, setDishargeLimit3)
{
   can::StandardDataFrame frame("1dc#0000000000000000");
   BatteryPowerLimits battery(frame);

   battery.setDischargePowerLimit_kW(99.9);

   EXPECT_FLOAT_EQ(99.9, battery.getDischargePowerLimit_kW());
}

TEST(NissanBatteryPowerLimits, getChargeLimit1)
{
   can::StandardDataFrame frame("1dc#EFCC7FFD0540C696");
   BatteryPowerLimits battery(frame);

   EXPECT_FLOAT_EQ(49.75, battery.getChargePowerLimit_kW());
}

TEST(NissanBatteryPowerLimits, getChargeLimit2)
{
   can::StandardDataFrame frame("1dc#EFC4FFFD0540C696");
   BatteryPowerLimits battery(frame);

   EXPECT_FLOAT_EQ(19.75, battery.getChargePowerLimit_kW());
}

TEST(NissanBatteryPowerLimits, setChargeLimit3)
{
   can::StandardDataFrame frame("1dc#0000000000000000");
   BatteryPowerLimits battery(frame);

   battery.setChargePowerLimit_kW(101.1);

   EXPECT_FLOAT_EQ(101.1, battery.getChargePowerLimit_kW());
}


