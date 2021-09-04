#include <gmock/gmock.h>
#include "can/StandardDataFrame.hpp"
#include "can/messages/TSUN/SleepAwakeCommand.hpp"
#include <vector>

using namespace can::messages::TSUN;
using namespace testing;


TEST(TsunSleepAwakeCommand, valid)
{
   EXPECT_TRUE(SleepAwakeCommand(can::StandardDataFrame("8200#5500000000000000")).valid());
}

TEST(TsunSleepAwakeCommand, invalidInfoType)
{
   EXPECT_FALSE(SleepAwakeCommand(can::StandardDataFrame("8200#0000000000000000")).valid());
}

TEST(TsunSleepAwakeCommand, invalidId)
{
   EXPECT_FALSE(SleepAwakeCommand(can::StandardDataFrame("8201#5500000000000000")).valid());
}

TEST(TsunSleepAwakeCommand, invalidLength)
{
   EXPECT_FALSE(SleepAwakeCommand(can::StandardDataFrame("8200#55000000000000")).valid());
}

TEST(TsunSleepAwakeCommand, commandEnterSleep)
{
   EXPECT_EQ(SleepAwakeCommand::ENTERSLEEP,SleepAwakeCommand(can::StandardDataFrame("8200#5500000000000000")).getCommand());
}

TEST(TsunSleepAwakeCommand, commandQuitSleep)
{
   EXPECT_EQ(SleepAwakeCommand::QUITSLEEP,SleepAwakeCommand(can::StandardDataFrame("8200#AA00000000000000")).getCommand());
}
