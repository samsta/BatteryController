#include <gmock/gmock.h>
#include "can/messages/SINEX/InverterHeartbeat.hpp"
#include "can/StandardDataFrame.hpp"
#include <vector>

using namespace can::messages::SINEX;
using namespace testing;


TEST(SinexInverterHeartbeat, valid)
{
   EXPECT_TRUE(InverterHeartbeat(can::StandardDataFrame("98160127#0000000000000000")).valid());
}

TEST(SinexInverterHeartbeat, inValid)
{
   EXPECT_FALSE(InverterHeartbeat(can::StandardDataFrame("98160128#0000000000000000")).valid());
}

TEST(SinexInverterHeartbeat, inValidSize)
{
   EXPECT_FALSE(InverterHeartbeat(can::StandardDataFrame("98160127#00000000000000")).valid());
}

TEST(SinexInverterHeartbeat, decodeValue1)
{
   InverterHeartbeat inverterhb(can::StandardDataFrame("98160127#FEFF000000000000"));
   EXPECT_TRUE(inverterhb.valid());
   EXPECT_EQ(inverterhb.getHeartbeatValue(),0xFFFE);
}

TEST(SinexInverterHeartbeat, decodeValue2)
{
   InverterHeartbeat inverterhb(can::StandardDataFrame("98160127#0100000000000000"));
   EXPECT_TRUE(inverterhb.valid());
   EXPECT_EQ(inverterhb.getHeartbeatValue(), 1);
}
