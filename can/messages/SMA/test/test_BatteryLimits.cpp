#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/SMA/BatteryLimits.hpp"
#include "can/messages/SMA/Ids.hpp"

using namespace can::messages::SMA;
using namespace can;
using namespace testing;

namespace {

TEST(SmaBatteryLimits, defaultInitialised)
{
   BatteryLimits battery_limits = BatteryLimits();

   EXPECT_EQ(ID_BATTERY_LIMITS, battery_limits.id());
   EXPECT_EQ(8, battery_limits.size());
   EXPECT_THAT(battery_limits, Each(0x00));
}

TEST(SmaBatteryLimits, exampleMessage)
{
   BatteryLimits battery_limits(495, 435, 16.2, 39.7);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_LIMITS, "135610FE018D00A2").str(), battery_limits.str());
}

TEST(SmaBatteryLimits, saturateAtMaximum)
{
   BatteryLimits limit(6553.5, 6553.5, 6553.5, 6553.5);
   BatteryLimits below_limit(6553.4, 6553.4, 6553.4, 6553.4);
   BatteryLimits limited(70000, 70000, 70000, 70000);

   EXPECT_EQ(limit.str(), limited.str());
   EXPECT_NE(limit.str(), below_limit.str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_LIMITS, "FFFFFFFFFFFFFFFF").str(), limit.str());
}

TEST(SmaBatteryLimits, cannotGoNegative)
{
   BatteryLimits limit(0, 0, 0, 0);
   BatteryLimits limited(-1, -2, -3, -4);

   EXPECT_EQ(limit.str(), limited.str());
}

}
