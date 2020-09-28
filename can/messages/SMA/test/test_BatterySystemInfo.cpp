#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/SMA/BatterySystemInfo.hpp"
#include "can/messages/SMA/Ids.hpp"

using namespace can::messages::SMA;
using namespace can;
using namespace testing;

namespace {

TEST(SmaBatterySystemInfo, defaultInitialised)
{
   BatterySystemInfo battery_system_info = BatterySystemInfo();

   EXPECT_EQ(ID_BATTERY_SYSTEM_INFO, battery_system_info.id());
   EXPECT_EQ(8, battery_system_info.size());
   EXPECT_THAT(battery_system_info, Each(0x00));
}

TEST(SmaBatterySystemInfo, exampleMessage)
{
   BatterySystemInfo battery_system_info = BatterySystemInfo(370673156, 9.8, 1, 2);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_SYSTEM_INFO, "1618060400620102").str(), battery_system_info.str());
}

TEST(SmaBatterySystemInfo, capacityCannotGoNegative)
{
   EXPECT_EQ(BatterySystemInfo().setCapacityKwh(0).str(),
             BatterySystemInfo().setCapacityKwh(-123).str());
}

TEST(SmaBatterySystemInfo, capacitySaturatesAtMaximum)
{
   EXPECT_EQ(BatterySystemInfo().setCapacityKwh(6553.5).str(),
             BatterySystemInfo().setCapacityKwh(70000).str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_SYSTEM_INFO, "00000000FFFF0000").str(),
             BatterySystemInfo().setCapacityKwh(6553.5).str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_SYSTEM_INFO, "00000000FFFE0000").str(),
             BatterySystemInfo().setCapacityKwh(6553.4).str());
}


}
