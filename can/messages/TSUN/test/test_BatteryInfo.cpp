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
   BatteryInfo battery_info(495, 32, 64);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_INFO, "5613000000002040").str(), battery_info.str());
}

TEST(TsunBatteryInfo, saturateAtMaximum)
{
   BatteryInfo limit(6553.5, 255, 255);
   BatteryInfo below_limit(6553.4, 254, 254);
   BatteryInfo limited(7000, 300, 300);

   EXPECT_EQ(limit.str(), limited.str());
   EXPECT_NE(limit.str(), below_limit.str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_INFO, "FFFF00000000FFFF").str(), limit.str());
}

TEST(TsunBatteryInfo, cannotGoNegative)
{
   BatteryInfo limit(0, 0, 0);
   BatteryInfo limited(-1, 0, 0);

   EXPECT_EQ(limit.str(), limited.str());
}

}
