#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/SMA/BatteryIdentity.hpp"
#include "can/messages/SMA/Ids.hpp"

using namespace can::messages::SMA;
using namespace can;
using namespace testing;

namespace {

TEST(SmaBatteryIdentity, defaultInitialised)
{
   BatteryIdentity battery_identity = BatteryIdentity();

   EXPECT_EQ(ID_BATTERY_IDENTITY, battery_identity.id());
   EXPECT_EQ(8, battery_identity.size());
   EXPECT_THAT(battery_identity, Each(0x00));
}

TEST(SmaBatteryIdentity, exampleMessage)
{
   BatteryIdentity battery_identity = BatteryIdentity(0x12345678, 0x87654321);

   EXPECT_EQ(StandardDataFrame(ID_BATTERY_IDENTITY, "1234567887654321").str(), battery_identity.str());
}

}
