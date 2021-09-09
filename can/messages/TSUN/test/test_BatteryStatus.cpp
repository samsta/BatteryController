#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/TSUN/BatteryStatus.hpp"
#include "can/messages/TSUN/Ids.hpp"

using namespace can::messages::TSUN;
using namespace can;
using namespace testing;

namespace {

TEST(TsunBatteryStatus, defaultInitialised)
{
   BatteryStatus battery_status = BatteryStatus();

   EXPECT_EQ(ID_BATTERY_STATUS, battery_status.id());
   EXPECT_EQ(8, battery_status.size());
   EXPECT_THAT(battery_status, Each(0x00));
}

TEST(TsunBatteryStatus, exampleMessage1)
{
   BatteryStatus battery_info(can::messages::TSUN::BatteryStatus::BASIC_STATUS_IDLE,
                              can::messages::TSUN::BatteryStatus::NO_ALARM);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0300000000000000").str(), battery_info.str());
}

TEST(TsunBatteryStatus, exampleMessage2)
{
   can::messages::TSUN::BatteryStatus::Alarm alarms = can::messages::TSUN::BatteryStatus::MOD_HIGH_VOLTAGE;// | can::messages::TSUN::BatteryStatus::DISCHARGE_CELL_HIGH_TEMP;
   BatteryStatus battery_info(can::messages::TSUN::BatteryStatus::BASIC_STATUS_IDLE,
                              alarms);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0300000000040000").str(), battery_info.str());
}


//TEST(TsunBatteryStatus, cannotGoNegative)
//{
//   BatteryStatus limit(0, 0, 0);
//   BatteryStatus limited(-1, 0, 0);
//
//   EXPECT_EQ(limit.str(), limited.str());
//}

}
