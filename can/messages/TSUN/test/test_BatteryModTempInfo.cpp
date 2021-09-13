#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/TSUN/BatteryModTempInfo.hpp"
#include "can/messages/TSUN/Ids.hpp"

using namespace can::messages::TSUN;
using namespace can;
using namespace testing;

namespace {

TEST(TsunBatteryModTempInfo, defaultInitialised)
{
   BatteryModTempInfo battery_limits = BatteryModTempInfo();

   EXPECT_EQ(ID_BATTERY_MODULE_TEMP_INFO, battery_limits.id());
   EXPECT_EQ(8, battery_limits.size());
   EXPECT_THAT(battery_limits, Each(0x00));
}

TEST(TsunBatteryModTempInfo, exampleMessage)
{
   BatteryModTempInfo battery_info(23.3, -2.2, 1, 2);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_MODULE_TEMP_INFO, "D104D20301000200").str(), battery_info.str());
}

TEST(TsunBatteryModTempInfo, saturateAtMaximum)
{
   BatteryModTempInfo limited(7000, 7000, 70000, 70000);
   BatteryModTempInfo below_limit(6453.4, 6453.4, 65534, 65534);
   BatteryModTempInfo limit(6453.5, 6453.5, 65535, 65535);

   EXPECT_EQ(limit.str(), limited.str());
   EXPECT_NE(limit.str(), below_limit.str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_MODULE_TEMP_INFO, "FFFFFFFFFFFFFFFF").str(), limit.str());
}

TEST(TsunBatteryModTempInfo, cannotGoNegative)
{
   BatteryModTempInfo limit(-100, -100, 0, 0);
   BatteryModTempInfo limited(-101, -101, -1, -1);

   EXPECT_EQ(limit.str(), limited.str());
}

}
