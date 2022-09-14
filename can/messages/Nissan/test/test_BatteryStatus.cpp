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
   EXPECT_FLOAT_EQ(56.0, status.getUsableSOC());

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
   EXPECT_EQ(string.str(), "BatteryStatus: Current=0.5A Voltage=381.5V Usable SOC=56% SecurityByte=0xE0 MultiplexByte=0x03");
}

TEST(NissanBatteryStatus, getNegativeCurrent)
{
   BatteryStatus status(can::StandardDataFrame("1db#FFE0FFFD0540C696"));

   EXPECT_FLOAT_EQ(-0.5, status.getCurrent());
}

TEST(NissanBatteryStatus, getUsableSOC)
{
   BatteryStatus status(can::StandardDataFrame("1db#F0E0F0FD9B40C696"));

   EXPECT_FLOAT_EQ(27.0, (float)status.getUsableSOC());
}

TEST(NissanBatteryStatusFlags, getStatusFlags)
{
   BatteryStatus status(can::StandardDataFrame("1db#0007000000000000"));

   EXPECT_EQ( 7, status.getFailsafeStatus());
}
