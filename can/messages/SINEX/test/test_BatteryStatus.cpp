#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/SINEX/BatteryStatus.hpp"
#include "can/messages/SINEX/Ids.hpp"

using namespace can::messages::SINEX;
using namespace can;
using namespace testing;
namespace {

TEST(SinexBatteryStatus, defaultInitialised)
{
   BatteryStatus battery_status = BatteryStatus();

   EXPECT_EQ(ID_BATTERY_STATUS, battery_status.id());
   EXPECT_EQ(8, battery_status.size());
   EXPECT_THAT(battery_status, Each(0x00));
}

TEST(SinexBatteryStatus, exampleMessage1)
{
   BatteryStatus battery_status = BatteryStatus();
   battery_status.setBatteryStatus( can::messages::SINEX::BatteryStatus::BatteryStatusFlag::BSF_DISCHARGE_CONTACTOR_CLOSED);
   battery_status.setBatteryStatus( can::messages::SINEX::BatteryStatus::BatteryStatusFlag::BSF_CHARGING_CONTACTOR_CLOSED);

   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "C000000000000000").str(), battery_status.str());

   battery_status.clearBatteryStatus( can::messages::SINEX::BatteryStatus::BatteryStatusFlag::BSF_DISCHARGE_CONTACTOR_CLOSED);
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "8000000000000000").str(), battery_status.str());

   battery_status.clearBatteryStatus( can::messages::SINEX::BatteryStatus::BatteryStatusFlag::BSF_CHARGING_CONTACTOR_CLOSED);
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());
}

TEST(SinexBatteryStatus, exampleMessage2)
{
   BatteryStatus battery_status( can::messages::SINEX::BatteryStatus::BatteryStatusFlag::BSF_DISCHARGE_CONTACTOR_CLOSED);

   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "4000000000000000").str(), battery_status.str());

   battery_status.clearBatteryStatus( can::messages::SINEX::BatteryStatus::BatteryStatusFlag::BSF_DISCHARGE_CONTACTOR_CLOSED);
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "000000000000000").str(), battery_status.str());
}


}
