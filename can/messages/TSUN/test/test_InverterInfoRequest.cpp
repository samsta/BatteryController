#include <gmock/gmock.h>
#include "can/messages/TSUN/InverterInfoRequest.hpp"
#include "can/StandardDataFrame.hpp"
#include <vector>

using namespace can::messages::TSUN;
using namespace testing;


TEST(TsunInverterInfoRequest, valid)
{
   EXPECT_TRUE(InverterInfoRequest(can::StandardDataFrame("80004200#0000000000000000")).valid());
}

TEST(TsunInverterInfoRequest, invalidInfoType)
{
   EXPECT_FALSE(InverterInfoRequest(can::StandardDataFrame("80004200#0100000000000000")).valid());
}

TEST(TsunInverterInfoRequest, invalidId)
{
   EXPECT_FALSE(InverterInfoRequest(can::StandardDataFrame("4201#0000000000000000")).valid());
}

TEST(TsunInverterInfoRequest, invalidLength)
{
   EXPECT_FALSE(InverterInfoRequest(can::StandardDataFrame("80004200#00000000000000")).valid());
}

TEST(TsunInverterInfoRequest, infotypeEnsemble)
{
   EXPECT_EQ(InverterInfoRequest::ENSEMBLE,InverterInfoRequest(can::StandardDataFrame("80004200#0000000000000000")).getInfoType());
}

TEST(TsunInverterInfoRequest, infotypeSystemEquipment)
{
   EXPECT_EQ(InverterInfoRequest::SYSTEM_EQUIPMENT,InverterInfoRequest(can::StandardDataFrame("80004200#0200000000000000")).getInfoType());
}


