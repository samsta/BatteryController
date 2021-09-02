#include <gmock/gmock.h>
#include "can/StandardDataFrame.hpp"
#include "can/messages/TSUN/SystemEquipment.hpp"
#include <vector>

using namespace can::messages::TSUN;
using namespace testing;


TEST(TsunSystemEquipment, valid)
{
   EXPECT_TRUE(SystemEquipment(can::StandardDataFrame("4200#0200000000000000")).valid());
}

TEST(TsunSystemEquipment, invalidTypeData)
{
   EXPECT_FALSE(SystemEquipment(can::StandardDataFrame("4200#0100000000000000")).valid());
}

TEST(TsunSystemEquipment, invalidId)
{
   EXPECT_FALSE(SystemEquipment(can::StandardDataFrame("4201#0200000000000000")).valid());
}

TEST(TsunSystemEquipment, invalidLength)
{
   EXPECT_FALSE(SystemEquipment(can::StandardDataFrame("4200#02000000000000")).valid());
}
