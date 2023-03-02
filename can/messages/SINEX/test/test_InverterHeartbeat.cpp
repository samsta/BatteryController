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



