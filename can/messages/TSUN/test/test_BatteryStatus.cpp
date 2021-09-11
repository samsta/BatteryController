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
   BatteryStatus battery_status(can::messages::TSUN::BatteryStatus::BASIC_STATUS_IDLE);

   EXPECT_FALSE(battery_status.hasAlarm());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0300000000000000").str(), battery_status.str());
}

TEST(TsunBatteryStatus, exampleMessage2)
{
   BatteryStatus battery_status(can::messages::TSUN::BatteryStatus::BASIC_STATUS_DISCHARGE);
   battery_status.setAlarm(can::messages::TSUN::BatteryStatus::SINGLE_CELL_LOW_VOLTAGE);

   EXPECT_TRUE(battery_status.hasAlarm());
   EXPECT_TRUE(battery_status.hasAlarm(can::messages::TSUN::BatteryStatus::SINGLE_CELL_LOW_VOLTAGE));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0200000001000000").str(), battery_status.str());
}

TEST(TsunBatteryStatus, exampleMessage3)
{
   BatteryStatus battery_status(can::messages::TSUN::BatteryStatus::BASIC_STATUS_CHARGE);
   battery_status.setAlarm( can::messages::TSUN::BatteryStatus::SINGLE_CELL_HIGH_VOLTAGE);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0100000002000000").str(), battery_status.str());
}

TEST(TsunBatteryStatus, exampleMessage4)
{
   BatteryStatus battery_status(can::messages::TSUN::BatteryStatus::BASIC_STATUS_SLEEP);
   battery_status.setAlarm( can::messages::TSUN::BatteryStatus::CHARGE_OVER_CURRENT);

   EXPECT_TRUE(battery_status.hasAlarm());
   EXPECT_TRUE(battery_status.hasAlarm(can::messages::TSUN::BatteryStatus::CHARGE_OVER_CURRENT));
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0000000000010000").str(), battery_status.str());
}

TEST(TsunBatteryStatus, exampleMessage5)
{
   BatteryStatus battery_status(can::messages::TSUN::BatteryStatus::BASIC_STATUS_IDLE);
   battery_status.setAlarm( can::messages::TSUN::BatteryStatus::DISCHARGE_OVER_CURRENT);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0300000000020000").str(), battery_status.str());
}

TEST(TsunBatteryStatus, exampleMessage6)
{
   BatteryStatus battery_status(can::messages::TSUN::BatteryStatus::BASIC_STATUS_IDLE);
   battery_status.setAlarm( can::messages::TSUN::BatteryStatus::DISCHARGE_CELL_HIGH_TEMP);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0300000080000000").str(), battery_status.str());
}

TEST(TsunBatteryStatus, exampleMessage7)
{
   BatteryStatus battery_status(can::messages::TSUN::BatteryStatus::BASIC_STATUS_IDLE);
   battery_status.setAlarm( can::messages::TSUN::BatteryStatus::MOD_LOW_VOLTAGE);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0300000000040000").str(), battery_status.str());
}

TEST(TsunBatteryStatus, exampleMessage8)
{
   BatteryStatus battery_status(can::messages::TSUN::BatteryStatus::BASIC_STATUS_IDLE);
   battery_status.setAlarm( can::messages::TSUN::BatteryStatus::MOD_LOW_VOLTAGE);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0300000000040000").str(), battery_status.str());
}

TEST(TsunBatteryStatus, exampleMessage9)
{
   BatteryStatus battery_status(can::messages::TSUN::BatteryStatus::BASIC_STATUS_IDLE);
   battery_status.setAlarm( can::messages::TSUN::BatteryStatus::SINGLE_CELL_LOW_VOLTAGE);
   battery_status.setAlarm( can::messages::TSUN::BatteryStatus::CHARGE_OVER_CURRENT);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0300000001010000").str(), battery_status.str());

   battery_status.clearAlarm(can::messages::TSUN::BatteryStatus::SINGLE_CELL_LOW_VOLTAGE);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATUS, "0300000000010000").str(), battery_status.str());

}

}
