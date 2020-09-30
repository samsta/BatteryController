#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/SMA/BatteryManufacturer.hpp"
#include "can/messages/SMA/BatteryName.hpp"
#include "can/messages/SMA/Ids.hpp"

using namespace can::messages::SMA;
using namespace can;
using namespace testing;

namespace {

TEST(SmaBatteryManufacturer, defaultInitialised)
{
   BatteryManufacturer battery_manufacturer = BatteryManufacturer();

   EXPECT_EQ(ID_BATTERY_MANUFACTURER, battery_manufacturer.id());
   EXPECT_EQ(8, battery_manufacturer.size());
   EXPECT_THAT(battery_manufacturer, Each(0x00));
}

TEST(SmaBatteryManufacturer, exampleMessages)
{
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_MANUFACTURER, "0057656C636F6D65").str(),
             BatteryManufacturer(0, "Welcome").str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_MANUFACTURER, "0148690000000000").str(),
             BatteryManufacturer(1, "Hi").str()) << "short string should be padded with nulls";
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_MANUFACTURER, "FF61626364656667").str(),
             BatteryManufacturer(255, "abcdefghijkl").str()) << "long string should be truncated";

}

TEST(SmaBatteryName, defaultInitialised)
{
   BatteryName battery_name = BatteryName();

   EXPECT_EQ(ID_BATTERY_NAME, battery_name.id());
   EXPECT_EQ(8, battery_name.size());
   EXPECT_THAT(battery_name, Each(0x00));
}

TEST(SmaBatteryName, exampleMessages)
{
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_NAME, "0057656C636F6D65").str(),
             BatteryName(0, "Welcome").str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_NAME, "0148690000000000").str(),
             BatteryName(1, "Hi").str()) << "short string should be padded with nulls";
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_NAME, "FF61626364656667").str(),
             BatteryName(255, "abcdefghijkl").str()) << "long string should be truncated";

}

}
