#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/TSUN/BatteryInfo.hpp"
#include "can/messages/TSUN/Ids.hpp"

using namespace can::messages::TSUN;
using namespace can;
using namespace testing;

namespace {

TEST(TsunBatteryInfo, defaultInitialised)
{
   BatteryInfo battery_limits = BatteryInfo();

   EXPECT_EQ(ID_BATTERY_INFO, battery_limits.id());
   EXPECT_EQ(8, battery_limits.size());
   EXPECT_THAT(battery_limits, Each(0x00));
}

TEST(TsunBatteryInfo, exampleMessage)
{
   BatteryInfo battery_info(495.0, 0.0, 0.0, 32, 64);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_INFO, "56133075E8032040").str(), battery_info.str());
}

TEST(TsunBatteryInfo, saturateAtMaximum)
{
   BatteryInfo limited(7000, 4000, 7000, 300, 300);
   BatteryInfo below_limit(6553.4, 3553.4, 6453.4, 254, 254);
   BatteryInfo limit(6553.5, 3553.5, 6453.5, 255, 255);

   EXPECT_EQ(limit.str(), limited.str());
   EXPECT_NE(limit.str(), below_limit.str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_INFO, "FFFFFFFFFFFFFFFF").str(), limit.str());
}

TEST(TsunBatteryInfo, cannotGoNegative)
{
   BatteryInfo limit(0, -3001, -101, 0, 0);
   BatteryInfo limited(-1, -3000, -100, 0, 0);

   EXPECT_EQ(limit.str(), limited.str());
}

}
