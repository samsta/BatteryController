#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/TSUN/BatteryCellVoltInfo.hpp"
#include "can/messages/TSUN/Ids.hpp"

using namespace can::messages::TSUN;
using namespace can;
using namespace testing;

namespace {

TEST(TsunBatteryCellVoltInfo, defaultInitialised)
{
   BatteryCellVoltInfo battery_limits = BatteryCellVoltInfo();

   EXPECT_EQ(ID_BATTERY_CELL_VOLT_INFO, battery_limits.id());
   EXPECT_EQ(8, battery_limits.size());
   EXPECT_THAT(battery_limits, Each(0x00));
}

TEST(TsunBatteryCellVoltInfo, exampleMessage)
{
   BatteryCellVoltInfo battery_info(3.980, 3.978, 1, 2);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_CELL_VOLT_INFO, "8C0F8A0F01000200").str(), battery_info.str());
}

TEST(TsunBatteryCellVoltInfo, saturateAtMaximum)
{
   BatteryCellVoltInfo limited(70, 700, 70000, 70000);
   BatteryCellVoltInfo below_limit(65.534, 65.534, 65534, 65534);
   BatteryCellVoltInfo limit(65.535, 65.535, 65535, 65535);

   EXPECT_EQ(limit.str(), limited.str());
   EXPECT_NE(limit.str(), below_limit.str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_CELL_VOLT_INFO, "FFFFFFFFFFFFFFFF").str(), limit.str());
}

TEST(TsunBatteryCellVoltInfo, cannotGoNegative)
{
   BatteryCellVoltInfo limit(0, 0, 0, 0);
   BatteryCellVoltInfo limited(-1, -1, -1, -1);

   EXPECT_EQ(limit.str(), limited.str());
}

}
