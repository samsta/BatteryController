#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/SINEX/BatteryLimitsOne.hpp"
#include "can/messages/SINEX/Ids.hpp"

using namespace can::messages::SINEX;
using namespace can;
using namespace testing;

namespace {

TEST(SinexBatteryLimitsOne, defaultInitialised)
{
   BatteryLimitsOne battery_limits = BatteryLimitsOne();

   EXPECT_EQ(ID_BATTERY_LIMITS_ONE, battery_limits.id());
   EXPECT_EQ(8, battery_limits.size());
   EXPECT_THAT(battery_limits, Each(0x00));
}

TEST(SinexBatteryLimits, exampleMessage)
{
   BatteryLimitsOne battery_limits(0.1, 0.2, 0.3, 0.4);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_LIMITS_ONE, "0100020003000400").str(), battery_limits.str());
}

TEST(SinexBatteryLimits, saturateAtMaximum)
{
   BatteryLimitsOne limit(6553.5, 6553.5, 6553.5, 6553.5);
   BatteryLimitsOne below_limit(6553.4, 6553.4, 6553.4, 6553.4);
   BatteryLimitsOne limited(7000, 7000, 7000, 7000);

   EXPECT_EQ(limit.str(), limited.str());
   EXPECT_NE(limit.str(), below_limit.str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_LIMITS_ONE, "FFFFFFFFFFFFFFFF").str(), limit.str());
}

TEST(SinexBatteryLimits, cannotGoNegative)
{
   BatteryLimitsOne limit(0, 0, 0, -0);
   BatteryLimitsOne limited(-1, -2, -3, -4);

   EXPECT_EQ(limit.str(), limited.str());
}

}
