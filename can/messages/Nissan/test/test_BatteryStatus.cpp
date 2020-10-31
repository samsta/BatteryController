#include <gmock/gmock.h>
#include "can/StandardDataFrame.hpp"
#include "can/messages/Nissan/BatteryStatus.hpp"

using namespace can::messages::Nissan;
using namespace testing;

TEST(NissanBatteryStatus, valid)
{
   EXPECT_TRUE(BatteryStatus(can::StandardDataFrame("1db#0000000000000000")).valid());
}

TEST(NissanBatteryStatus, invalidId)
{
   EXPECT_FALSE(BatteryStatus(can::StandardDataFrame("1dc#0000000000000000")).valid());
}

TEST(NissanBatteryStatus, invalidSize)
{
   EXPECT_FALSE(BatteryStatus(can::StandardDataFrame("1db#00000000000000")).valid());
}

TEST(NissanBatteryStatus, exampleMessage)
{
   BatteryStatus status(can::StandardDataFrame("1db#0024bee2380003e0"));
   
   EXPECT_FLOAT_EQ(0.5, status.getCurrent());
   EXPECT_FLOAT_EQ(381.5, status.getVoltage());
   EXPECT_EQ(224, status.getSecurityByte());
   EXPECT_EQ(3, status.getMultiplexByte());
}

TEST(NissanBatteryStatus, invalidToString)
{
   BatteryStatus status(can::StandardDataFrame("1dc#0000000000000000"));
   std::ostringstream string;
   string << status;
   EXPECT_EQ(string.str(), "BatteryStatus: invalid");
}

TEST(NissanBatteryStatus, validToString)
{
   BatteryStatus status(can::StandardDataFrame("1db#0024bee2380003e0"));
   std::ostringstream string;
   string << status;
   EXPECT_EQ(string.str(), "BatteryStatus: Current=0.5A Voltage=381.5V SecurityByte=0xE0 MultiplexByte=0x03");
}