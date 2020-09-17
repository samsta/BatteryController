#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/SMA/BatteryState.hpp"
#include "can/messages/SMA/Ids.hpp"

using namespace can::messages::SMA;
using namespace can;
using namespace testing;

namespace {

TEST(SmaBatteryState, defaultInitialised)
{
   BatteryState battery_state = BatteryState();

   EXPECT_EQ(ID_BATTERY_STATE, battery_state.id());
   EXPECT_EQ(8, battery_state.size());
   EXPECT_THAT(battery_state, Each(0x00));
}

TEST(SmaBatteryState, exampleMessage)
{
   BatteryState battery_state(54.99, 97.62, 46.6, 90.29);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATE, "157B262212342345").str(), battery_state.str());
}

   
}
