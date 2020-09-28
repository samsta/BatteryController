#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/SMA/BatteryMeasurements.hpp"
#include "can/messages/SMA/Ids.hpp"

using namespace can::messages::SMA;
using namespace can;
using namespace testing;

namespace {

TEST(SmaBatteryMeasurements, defaultInitialises)
{
   BatteryMeasurements battery_measurements = BatteryMeasurements();

   EXPECT_EQ(ID_BATTERY_MEASUREMENTS, battery_measurements.id());
   EXPECT_EQ(8, battery_measurements.size());
   EXPECT_THAT(battery_measurements, Each(0x0));
}

TEST(SmaBatteryMeasurements, exampleMessage)
{
   BatteryMeasurements measurements(469.9, 1.6, 14.1, 3, 0x16);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_MEASUREMENTS, "125B0010008D0316").str(), measurements.str());
}

TEST(SmaBatteryMeasurements, voltageCannotGoNegative)
{
   BatteryMeasurements limited{};
   limited.setVoltage(-10);

   EXPECT_EQ(BatteryMeasurements().str(), limited.str());
}

   TEST(SmaBatteryMeasurements, voltageSaturatesAtMaximum)
{
   BatteryMeasurements limited{};
   limited.setVoltage(66000);

   EXPECT_EQ(BatteryMeasurements().setVoltage(6553.5).str(), limited.str());
}

TEST(SmaBatteryMeasurements, currentSaturatesAtMinimum)
{
   BatteryMeasurements limited{};
   limited.setCurrent(-60000);

   EXPECT_EQ(BatteryMeasurements().setCurrent(-3276.8).str(), limited.str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_MEASUREMENTS, "0000800000000000").str(),
             BatteryMeasurements().setCurrent(-3276.8).str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_MEASUREMENTS, "0000800100000000").str(),
             BatteryMeasurements().setCurrent(-3276.7).str());
}

TEST(SmaBatteryMeasurements, currentSaturatesAtMaximum)
{
   BatteryMeasurements limited{};
   limited.setCurrent(60000);

   EXPECT_EQ(BatteryMeasurements().setCurrent(3276.7).str(), limited.str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_MEASUREMENTS, "00007FFF00000000").str(),
             BatteryMeasurements().setCurrent(3276.7).str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_MEASUREMENTS, "00007FFE00000000").str(),
             BatteryMeasurements().setCurrent(3276.6).str());
}

TEST(SmaBatteryMeasurements, temperatureSaturatesAtMinimum)
{
   BatteryMeasurements limited{};
   limited.setTemperature(-60000);

   EXPECT_EQ(BatteryMeasurements().setTemperature(-3276.8).str(), limited.str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_MEASUREMENTS, "0000000080000000").str(),
             BatteryMeasurements().setTemperature(-3276.8).str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_MEASUREMENTS, "0000000080010000").str(),
             BatteryMeasurements().setTemperature(-3276.7).str());
}

TEST(SmaBatteryMeasurements, temperatureSaturatesAtMaximum)
{
   BatteryMeasurements limited{};
   limited.setTemperature(60000);

   EXPECT_EQ(BatteryMeasurements().setTemperature(3276.7).str(), limited.str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_MEASUREMENTS, "000000007FFF0000").str(),
             BatteryMeasurements().setTemperature(3276.7).str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_MEASUREMENTS, "000000007FFE0000").str(),
             BatteryMeasurements().setTemperature(3276.6).str());
}

   
}
