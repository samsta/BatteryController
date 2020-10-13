#include <gmock/gmock.h>
#include "can/StandardDataFrame.hpp"
#include "can/messages/SMA/InverterIdentity.hpp"
#include <vector>

using namespace can::messages::SMA;
using namespace testing;


TEST(SmaInverterIdentity, valid)
{
   EXPECT_TRUE(InverterIdentity(can::StandardDataFrame("560#0000000000000000")).valid());
}

TEST(SmaInverterIdentity, invalidId)
{
   EXPECT_FALSE(InverterIdentity(can::StandardDataFrame("561#0000000000000000")).valid());
}

TEST(SmaInverterIdentity, invalidLength)
{
   EXPECT_FALSE(InverterIdentity(can::StandardDataFrame("560#00000000000000")).valid());
}

TEST(SmaInverterIdentity, exampleMessage)
{
   InverterIdentity identity(can::StandardDataFrame("560#01501A0401680400"));

   EXPECT_EQ(22026756, identity.getSoftwareVersion());
   EXPECT_EQ(360, identity.getInverterId());
   EXPECT_EQ(4, identity.getConfiguration());
}

TEST(SmaInverterIdentity, invalidToString)
{
   InverterIdentity command(can::StandardDataFrame("361#"));
   std::ostringstream string;
   string << command;
   EXPECT_EQ(string.str(), "InverterIdentity: invalid");
}

TEST(SmaInverterIdentity, validToString)
{
   InverterIdentity identity(can::StandardDataFrame("560#01501A0401680400"));
   std::ostringstream string;
   string << identity;
   EXPECT_EQ(string.str(), "InverterIdentity: SoftwareVersion=22026756 InverterId=360 Configuration=0x04");
}

