#include <gmock/gmock.h>
#include "can/messages/TSUN/InverterSleepAwakeCmd.hpp"
#include "can/StandardDataFrame.hpp"
#include <vector>

using namespace can::messages::TSUN;
using namespace testing;


TEST(TsunInverterSleepAwakeCmd, valid)
{
   EXPECT_TRUE(InverterSleepAwakeCmd(can::StandardDataFrame("00008201#5500000000000000")).valid());
}

TEST(TsunInverterSleepAwakeCmd, invalidInfoType)
{
   EXPECT_FALSE(InverterSleepAwakeCmd(can::StandardDataFrame("00008201#0000000000000000")).valid());
}

TEST(TsunInverterSleepAwakeCmd, invalidId)
{
   EXPECT_FALSE(InverterSleepAwakeCmd(can::StandardDataFrame("8202#5500000000000000")).valid());
}

TEST(TsunInverterSleepAwakeCmd, invalidLength)
{
   EXPECT_FALSE(InverterSleepAwakeCmd(can::StandardDataFrame("00008201#55000000000000")).valid());
}

TEST(TsunInverterSleepAwakeCmd, commandEnterSleep)
{
   EXPECT_EQ(InverterSleepAwakeCmd::ENTER_SLEEP,InverterSleepAwakeCmd(can::StandardDataFrame("00008201#5500000000000000")).getCommand());
}

TEST(TsunInverterSleepAwakeCmd, commandQuitSleep)
{
   EXPECT_EQ(InverterSleepAwakeCmd::QUIT_SLEEP,InverterSleepAwakeCmd(can::StandardDataFrame("00008201#AA00000000000000")).getCommand());
}
