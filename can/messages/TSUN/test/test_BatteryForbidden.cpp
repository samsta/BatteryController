#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/TSUN/BatteryForbidden.hpp"
#include "can/messages/TSUN/Ids.hpp"

using namespace can::messages::TSUN;
using namespace can;
using namespace testing;

namespace {

TEST(TsunBatteryForbidden, defaultInitialised)
{
   BatteryForbidden battery_limits = BatteryForbidden();

   EXPECT_EQ(ID_BATTERY_FORBIDDEN, battery_limits.id());
   EXPECT_EQ(8, battery_limits.size());
   EXPECT_THAT(battery_limits, Each(0x00));
}

}
