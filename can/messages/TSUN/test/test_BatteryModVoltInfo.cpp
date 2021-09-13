#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/TSUN/BatteryModVoltInfo.hpp"
#include "can/messages/TSUN/Ids.hpp"

using namespace can::messages::TSUN;
using namespace can;
using namespace testing;

namespace {

TEST(TsunBatteryModVoltInfo, defaultInitialised)
{
   BatteryModVoltInfo battery_limits = BatteryModVoltInfo();

   EXPECT_EQ(ID_BATTERY_MODULE_VOLT_INFO, battery_limits.id());
   EXPECT_EQ(8, battery_limits.size());
   EXPECT_THAT(battery_limits, Each(0x00));
}

TEST(TsunBatteryModVoltInfo, exampleMessage)
{
   BatteryModVoltInfo battery_info(7.960, 7.956, 1, 2);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_MODULE_VOLT_INFO, "181F141F01000200").str(), battery_info.str());
}

TEST(TsunBatteryModVoltInfo, saturateAtMaximum)
{
   BatteryModVoltInfo limited(70, 70, 70000, 70000);
   BatteryModVoltInfo below_limit(65.534, 65.534, 65534, 65534);
   BatteryModVoltInfo limit(65.535, 65.535, 65535, 65535);

   EXPECT_EQ(limit.str(), limited.str());
   EXPECT_NE(limit.str(), below_limit.str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_MODULE_VOLT_INFO, "FFFFFFFFFFFFFFFF").str(), limit.str());
}

TEST(TsunBatteryModVoltInfo, cannotGoNegative)
{
   BatteryModVoltInfo limit(0, 0, 0, 0);
   BatteryModVoltInfo limited(-1, -1, -1, -1);

   EXPECT_EQ(limit.str(), limited.str());
}

}
