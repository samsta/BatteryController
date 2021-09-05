#include <gmock/gmock.h>
#include "can/StandardDataFrame.hpp"
#include "can/messages/TSUN/InformationRequest.hpp"
#include <vector>

using namespace can::messages::TSUN;
using namespace testing;


TEST(TsunInformationRequest, valid)
{
   EXPECT_TRUE(InformationRequest(can::StandardDataFrame("4200#0000000000000000")).valid());
}

TEST(TsunInformationRequest, invalidInfoType)
{
   EXPECT_FALSE(InformationRequest(can::StandardDataFrame("4200#0100000000000000")).valid());
}

TEST(TsunInformationRequest, invalidId)
{
   EXPECT_FALSE(InformationRequest(can::StandardDataFrame("4201#0000000000000000")).valid());
}

TEST(TsunInformationRequest, invalidLength)
{
   EXPECT_FALSE(InformationRequest(can::StandardDataFrame("4200#00000000000000")).valid());
}

TEST(TsunInformationRequest, infotypeEnsemble)
{
   EXPECT_EQ(InformationRequest::ENSEMBLE,InformationRequest(can::StandardDataFrame("4200#0000000000000000")).getInfoType());
}

TEST(TsunInformationRequest, infotypeSystemEquipment)
{
   EXPECT_EQ(InformationRequest::SYSTEM_EQUIPMENT,InformationRequest(can::StandardDataFrame("4200#0200000000000000")).getInfoType());
}


