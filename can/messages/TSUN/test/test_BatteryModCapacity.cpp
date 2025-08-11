#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/TSUN/BatteryModCapacity.hpp"
#include "can/messages/TSUN/Ids.hpp"

using namespace can::messages::TSUN;
using namespace can;
using namespace testing;

namespace {

TEST(TsunBatteryModCapacity, defaultInitialised)
{
   BatteryModCapacity battery_mod_capacity = BatteryModCapacity();

   EXPECT_EQ(ID_BATTERY_MODULE_CAPACITY, battery_mod_capacity.id());
   EXPECT_EQ(8, battery_mod_capacity.size());
   EXPECT_THAT(battery_mod_capacity, Each(0x00));
}

TEST(TsunBatteryModCapacity, exampleMessage)
{
   BatteryModCapacity battery_mod_capacity(120.0, 4.0, 30.0, 384.0, 37.0);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_MODULE_CAPACITY, "7800041E80012500").str(), battery_mod_capacity.str());
}

}
