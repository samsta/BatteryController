#include <gmock/gmock.h>
#include "can/StandardDataFrame.hpp"
#include "can/messages/TSUN/ChargeDischargeCommand.hpp"
#include <vector>

using namespace can::messages::TSUN;
using namespace testing;


TEST(TsunChargeDischargeCommand, valid)
{
   EXPECT_TRUE(ChargeDischargeCommand(can::StandardDataFrame("8210#AA00000000000000")).valid());
}

TEST(TsunChargeDischargeCommand, invalidId)
{
   EXPECT_FALSE(ChargeDischargeCommand(can::StandardDataFrame("8211#AA00000000000000")).valid());
}

TEST(TsunChargeDischargeCommand, invalidLength)
{
   EXPECT_FALSE(ChargeDischargeCommand(can::StandardDataFrame("8210#AA000000000000")).valid());
}

TEST(TsunChargeDischargeCommand, commandCharge)
{
   EXPECT_EQ(ChargeDischargeCommand::CHARGE,ChargeDischargeCommand(can::StandardDataFrame("8210#AA00000000000000")).getCommand());
}

TEST(TsunChargeDischargeCommand, commandDischarge)
{
   EXPECT_EQ(ChargeDischargeCommand::DISCHARGE,ChargeDischargeCommand(can::StandardDataFrame("8210#00AA000000000000")).getCommand());
}

TEST(TsunChargeDischargeCommand, commandInvalidBoth)
{
   EXPECT_EQ(ChargeDischargeCommand::INVALID,ChargeDischargeCommand(can::StandardDataFrame("8210#AAAA000000000000")).getCommand());
}

TEST(TsunChargeDischargeCommand, commandInvalidNone)
{
   EXPECT_EQ(ChargeDischargeCommand::INVALID,ChargeDischargeCommand(can::StandardDataFrame("8210#0000000000000000")).getCommand());
}
