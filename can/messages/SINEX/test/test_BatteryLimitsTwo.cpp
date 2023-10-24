#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/SINEX/BatteryLimitsTwo.hpp"
#include "can/messages/SINEX/Ids.hpp"

using namespace can::messages::SINEX;
using namespace can;
using namespace testing;

namespace {

TEST(SinexBatteryLimitsTwo, defaultInitialised)
{
   BatteryLimitsTwo battery_limits = BatteryLimitsTwo();

   EXPECT_EQ(ID_BATTERY_LIMITS_TWO, battery_limits.id());
   EXPECT_EQ(8, battery_limits.size());
   EXPECT_THAT(battery_limits, Each(0x00));
}

TEST(SinexBatteryLimitsTwo, exampleMessage1)
{
   BatteryLimitsTwo battery_limits(1.6, 6.4, 25.6);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_LIMITS_TWO, "1000400000010000").str(), battery_limits.str());
}

TEST(SinexBatteryLimitsTwo, exampleMessage2)
{
   BatteryLimitsTwo battery_limits(81.2, 99.9 , 409.6);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_LIMITS_TWO, "2C03E70300100000").str(), battery_limits.str());
}

}
