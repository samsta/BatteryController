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

TEST(SmaBatteryState, percentagesSaturateAtMaximum)
{
   BatteryState limit(100, 100, 0, 0);
   BatteryState below_limit(99.99, 99.99, 0, 0);
   BatteryState limited(150, 150, 0, 0);

   EXPECT_EQ(limit.str(), limited.str());
   EXPECT_NE(limit.str(), below_limit.str());
}

TEST(SmaBatteryState, energiesSaturateAtMaximum)
{
   BatteryState limit(0, 0, 655.35, 655.35);
   BatteryState below_limit(0, 0, 655.34, 655.34);
   BatteryState limited(0, 0, 1000, 1000);

   EXPECT_EQ(limit.str(), limited.str());
   EXPECT_NE(limit.str(), below_limit.str());
   EXPECT_EQ(StandardDataFrame(ID_BATTERY_STATE, "00000000FFFFFFFF").str(), limit.str());
}

TEST(SmaBatteryState, numbersSaturateAtMinimum)
{
   BatteryState limit(0, 0, 0, 0);
   BatteryState limited(-1, -2, -3, -4);

   EXPECT_EQ(limit.str(), limited.str());
}


}
