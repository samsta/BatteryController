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

TEST(SinexBatteryStatus, exampleMsgContactors)
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

TEST(SinexBatteryStatus, exampleMsgContactorDis)
{
   BatteryStatus battery_status( can::messages::SINEX::BatteryStatus::BatteryStatusFlag::BSF_DISCHARGE_CONTACTOR_CLOSED);

   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "4000000000000000").str(), battery_status.str());

   battery_status.clearBatteryStatus( can::messages::SINEX::BatteryStatus::BatteryStatusFlag::BSF_DISCHARGE_CONTACTOR_CLOSED);
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "000000000000000").str(), battery_status.str());
}

// THERE ARE NO TESTS FOR MILD ALARMS AS IT IS UNLIKELY THEY WILL EVER BE USED

TEST(SinexBatteryStatus, exampleAlarmModerate1)
{
   BatteryStatus battery_status = BatteryStatus();

   // no moderate alarm
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());
   // no severe alarm
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());

   battery_status.setAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE, can::messages::SINEX::BatteryStatus::AlarmFlag::SINGLE_UNDER_VOLTAGE);
   // has any moderate alarm
   EXPECT_TRUE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000800000").str(), battery_status.str());
   // has specific alarm
   EXPECT_TRUE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE, can::messages::SINEX::BatteryStatus::AlarmFlag::SINGLE_UNDER_VOLTAGE));
   // still no severe alarm
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE ));
   // no more moderate alarm
   battery_status.clearAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE, can::messages::SINEX::BatteryStatus::AlarmFlag::SINGLE_UNDER_VOLTAGE);
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());
}

TEST(SinexBatteryStatus, exampleAlarmModerate2)
{
   BatteryStatus battery_status = BatteryStatus();

   // no moderate alarm
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());
   // no severe alarm
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());

   battery_status.setAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE, can::messages::SINEX::BatteryStatus::AlarmFlag::ALTERNATE_ALM_0);
   // has any moderate alarm
   EXPECT_TRUE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000001000000").str(), battery_status.str());
   // has specific alarm
   EXPECT_TRUE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE, can::messages::SINEX::BatteryStatus::AlarmFlag::ALTERNATE_ALM_0));
   // still no severe alarm
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE ));
   // no more moderate alarm
   battery_status.clearAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE, can::messages::SINEX::BatteryStatus::AlarmFlag::ALTERNATE_ALM_0);
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());
}

TEST(SinexBatteryStatus, exampleAlarmModerate3)
{
   BatteryStatus battery_status = BatteryStatus();

   // no moderate alarm
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());
   // no severe alarm
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());

   battery_status.setAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE, can::messages::SINEX::BatteryStatus::AlarmFlag::EXCESSIVE_VOLT_DIFF);
   // has any moderate alarm
   EXPECT_TRUE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "000000001000000").str(), battery_status.str());
   // has specific alarm
   EXPECT_TRUE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE, can::messages::SINEX::BatteryStatus::AlarmFlag::EXCESSIVE_VOLT_DIFF));
   // still no severe alarm
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE ));
   // no more moderate alarm
   battery_status.clearAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE, can::messages::SINEX::BatteryStatus::AlarmFlag::EXCESSIVE_VOLT_DIFF);
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());
}

TEST(SinexBatteryStatus, exampleAlarmSevere1)
{
   BatteryStatus battery_status = BatteryStatus();

   // no moderate alarm
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());
   // no severe alarm
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());

   battery_status.setAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE, can::messages::SINEX::BatteryStatus::AlarmFlag::SINGLE_UNDER_VOLTAGE);
   // has any severe alarm
   EXPECT_TRUE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000080").str(), battery_status.str());
   // has specific alarm
   EXPECT_TRUE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE, can::messages::SINEX::BatteryStatus::AlarmFlag::SINGLE_UNDER_VOLTAGE));
   // still no moderate alarm
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE ));
   // no more severe alarm
   battery_status.clearAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE, can::messages::SINEX::BatteryStatus::AlarmFlag::SINGLE_UNDER_VOLTAGE);
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());
}

TEST(SinexBatteryStatus, exampleAlarmSevere2)
{
   BatteryStatus battery_status = BatteryStatus();

   // no moderate alarm
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());
   // no severe alarm
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());

   battery_status.setAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE, can::messages::SINEX::BatteryStatus::AlarmFlag::ALTERNATE_ALM_0);
   // has any severe alarm
   EXPECT_TRUE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000100").str(), battery_status.str());
   // has specific alarm
   EXPECT_TRUE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE, can::messages::SINEX::BatteryStatus::AlarmFlag::ALTERNATE_ALM_0));
   // still no moderate alarm
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE ));
   // no more severe alarm
   battery_status.clearAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE, can::messages::SINEX::BatteryStatus::AlarmFlag::ALTERNATE_ALM_0);
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());
}

TEST(SinexBatteryStatus, exampleAlarmSevere3)
{
   BatteryStatus battery_status = BatteryStatus();

   // no moderate alarm
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());
   // no severe alarm
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());

   battery_status.setAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE, can::messages::SINEX::BatteryStatus::AlarmFlag::EXCESSIVE_VOLT_DIFF);
   // has any severe alarm
   EXPECT_TRUE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000001000").str(), battery_status.str());
   // has specific alarm
   EXPECT_TRUE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE, can::messages::SINEX::BatteryStatus::AlarmFlag::EXCESSIVE_VOLT_DIFF));
   // still no moderate alarm
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE ));
   // no more severe alarm
   battery_status.clearAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE, can::messages::SINEX::BatteryStatus::AlarmFlag::EXCESSIVE_VOLT_DIFF);
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());
}

TEST(SinexBatteryStatus, exampleMultiAlarmSevere)
{
   BatteryStatus battery_status = BatteryStatus();

   // no moderate alarm
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());

   battery_status.setAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE, can::messages::SINEX::BatteryStatus::AlarmFlag::ALTERNATE_ALM_0);
   battery_status.setAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE, can::messages::SINEX::BatteryStatus::AlarmFlag::EXCESSIVE_VOLT_DIFF);
   battery_status.setAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE, can::messages::SINEX::BatteryStatus::AlarmFlag::SINGLE_UNDER_VOLTAGE);
   // has any severe alarm
   EXPECT_TRUE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000001180").str(), battery_status.str());
   // has specific alarm
   EXPECT_TRUE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE, can::messages::SINEX::BatteryStatus::AlarmFlag::EXCESSIVE_VOLT_DIFF));
   // still no moderate alarm
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::MODERATE ));
   // no more severe alarm
   battery_status.clearAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE, can::messages::SINEX::BatteryStatus::AlarmFlag::EXCESSIVE_VOLT_DIFF);
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE, can::messages::SINEX::BatteryStatus::AlarmFlag::EXCESSIVE_VOLT_DIFF));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000180").str(), battery_status.str());
   battery_status.clearAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE, can::messages::SINEX::BatteryStatus::AlarmFlag::ALTERNATE_ALM_0);
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE, can::messages::SINEX::BatteryStatus::AlarmFlag::ALTERNATE_ALM_0));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000080").str(), battery_status.str());
   battery_status.clearAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE, can::messages::SINEX::BatteryStatus::AlarmFlag::SINGLE_UNDER_VOLTAGE);
   EXPECT_FALSE(battery_status.hasAlarm(can::messages::SINEX::BatteryStatus::AlarmLevel::SEVERE ));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000000000").str(), battery_status.str());
}


}
