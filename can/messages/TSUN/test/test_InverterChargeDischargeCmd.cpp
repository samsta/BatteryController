#include <gmock/gmock.h>
#include "can/messages/TSUN/InverterChargeDischargeCmd.hpp"
#include "can/StandardDataFrame.hpp"
#include <vector>

using namespace can::messages::TSUN;
using namespace testing;


TEST(TsunInverterChargeDischargeCmd, valid)
{
   EXPECT_TRUE(InverterChargeDischargeCmd(can::StandardDataFrame("00008211#AA00000000000000")).valid());
}

TEST(TsunInverterChargeDischargeCmd, invalidId)
{
   EXPECT_FALSE(InverterChargeDischargeCmd(can::StandardDataFrame("00008212#AA00000000000000")).valid());
}

TEST(TsunInverterChargeDischargeCmd, invalidLength)
{
   EXPECT_FALSE(InverterChargeDischargeCmd(can::StandardDataFrame("00008211#AA000000000000")).valid());
}

TEST(TsunInverterChargeDischargeCmd, commandCharge)
{
   EXPECT_EQ(InverterChargeDischargeCmd::CHARGE,InverterChargeDischargeCmd(can::StandardDataFrame("00008211#AA00000000000000")).getCommand());
}

TEST(TsunInverterChargeDischargeCmd, commandDischarge)
{
   EXPECT_EQ(InverterChargeDischargeCmd::DISCHARGE,InverterChargeDischargeCmd(can::StandardDataFrame("00008211#00AA000000000000")).getCommand());
}

TEST(TsunInverterChargeDischargeCmd, commandInvalidBoth)
{
   EXPECT_EQ(InverterChargeDischargeCmd::INVALID,InverterChargeDischargeCmd(can::StandardDataFrame("00008211#AAAA000000000000")).getCommand());
}

TEST(TsunInverterChargeDischargeCmd, commandInvalidNone)
{
   EXPECT_EQ(InverterChargeDischargeCmd::INVALID,InverterChargeDischargeCmd(can::StandardDataFrame("00008211#0000000000000000")).getCommand());
}
