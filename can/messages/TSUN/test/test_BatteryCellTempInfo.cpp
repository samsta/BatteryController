#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/TSUN/BatteryCellTempInfo.hpp"
#include "can/messages/TSUN/Ids.hpp"

using namespace can::messages::TSUN;
using namespace can;
using namespace testing;

namespace {

TEST(TsunBatteryCellTempInfo, defaultInitialised)
{
   BatteryCellTempInfo battery_limits = BatteryCellTempInfo();

   EXPECT_EQ(ID_BATTERY_CELL_TEMP_INFO, battery_limits.id());
   EXPECT_EQ(8, battery_limits.size());
   EXPECT_THAT(battery_limits, Each(0x00));
}

TEST(TsunBatteryCellTempInfo, exampleMessage)
{
   BatteryCellTempInfo battery_info(23.3, -2.2, 1, 2);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_CELL_TEMP_INFO, "D104D20301000200").str(), battery_info.str());
}

TEST(TsunBatteryCellTempInfo, saturateAtMaximum)
{
   BatteryCellTempInfo limited(7000, 7000, 70000, 70000);
   BatteryCellTempInfo below_limit(6453.4, 6453.4, 65534, 65534);
   BatteryCellTempInfo limit(6453.5, 6453.5, 65535, 65535);

   EXPECT_EQ(limit.str(), limited.str());
   EXPECT_NE(limit.str(), below_limit.str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_CELL_TEMP_INFO, "FFFFFFFFFFFFFFFF").str(), limit.str());
}

TEST(TsunBatteryCellTempInfo, cannotGoNegative)
{
   BatteryCellTempInfo limit(-100, -100, 0, 0);
   BatteryCellTempInfo limited(-101, -101, -1, -1);

   EXPECT_EQ(limit.str(), limited.str());
}

}
