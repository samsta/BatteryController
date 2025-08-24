#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/TSUN/BatteryLimits.hpp"
#include "can/messages/TSUN/Ids.hpp"

using namespace can::messages::TSUN;
using namespace can;
using namespace testing;

namespace {

TEST(TsunBatteryLimits, defaultInitialised)
{
   BatteryLimits battery_limits = BatteryLimits();

   EXPECT_EQ(ID_BATTERY_LIMITS, battery_limits.id());
   EXPECT_EQ(8, battery_limits.size());
   EXPECT_THAT(battery_limits, Each(0x00));
}

TEST(TsunBatteryLimits, exampleMessage)
{
   BatteryLimits battery_limits(495, 435, 39.7, 16.2);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_LIMITS, "5613FE10BD76D275").str(), battery_limits.str());
}

TEST(TsunBatteryLimits, saturateAtMaximum)
{
   BatteryLimits limit(6553.5, 6553.5, 3553.5, 3553.5);
   BatteryLimits below_limit(6553.4, 6553.4, 3553.4, 3553.4);
   BatteryLimits limited(7000, 7000, 4000, 4000);

   EXPECT_EQ(limit.str(), limited.str());
   EXPECT_NE(limit.str(), below_limit.str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_LIMITS, "FFFFFFFFFFFFFFFF").str(), limit.str());
}

TEST(TsunBatteryLimits, cannotGoNegative)
{
   BatteryLimits limit(0, 0, -3000, -3000);
   BatteryLimits limited(-1, -2, -3004, -3003);

   EXPECT_EQ(limit.str(), limited.str());
}

}
