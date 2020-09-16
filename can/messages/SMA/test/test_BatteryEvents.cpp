#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/SMA/BatteryEvents.hpp"
#include "can/messages/SMA/BatteryErrors.hpp"
#include "can/messages/SMA/BatteryAlarms.hpp"
#include "can/messages/SMA/Ids.hpp"

using namespace can::messages::SMA;
using namespace testing;

namespace {

TEST(SmaBatteryEvents, allOnes)
{
   BatteryEvents battery_events = BatteryEvents();

   EXPECT_EQ(ID_BATTERY_EVENTS, battery_events.id());
   EXPECT_EQ(8, battery_events.size());
   EXPECT_THAT(battery_events, Each(0xFF));
}

TEST(SmaBatteryErrors, allOnes)
{
   BatteryErrors battery_errors = BatteryErrors();

   EXPECT_EQ(ID_BATTERY_ERRORS, battery_errors.id());
   EXPECT_EQ(8, battery_errors.size());
   EXPECT_THAT(battery_errors, Each(0xFF));
}

TEST(SmaBatteryAlarms, allOnes)
{
   BatteryAlarms battery_alarms = BatteryAlarms();

   EXPECT_EQ(ID_BATTERY_ALARMS, battery_alarms.id());
   EXPECT_EQ(8, battery_alarms.size());
   EXPECT_THAT(battery_alarms, Each(0x00));
}

   
}
