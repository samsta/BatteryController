#include <gmock/gmock.h>
#include "can/StandardDataFrame.hpp"
#include "can/messages/TSUN/EnsembleInformation.hpp"
#include <vector>

using namespace can::messages::TSUN;
using namespace testing;


TEST(TsunEnsembleInformation, valid)
{
   EXPECT_TRUE(EnsembleInformation(can::StandardDataFrame("4200#0000000000000000")).valid());
}

TEST(TsunEnsembleInformation, invalidTypeData)
{
   EXPECT_FALSE(EnsembleInformation(can::StandardDataFrame("4200#0100000000000000")).valid());
}

TEST(TsunEnsembleInformation, invalidId)
{
   EXPECT_FALSE(EnsembleInformation(can::StandardDataFrame("4201#0000000000000000")).valid());
}

TEST(TsunEnsembleInformation, invalidLength)
{
   EXPECT_FALSE(EnsembleInformation(can::StandardDataFrame("4200#00000000000000")).valid());
}

TEST(TsunEnsembleInformation, isCorrectId)
{
	   EXPECT_TRUE(EnsembleInformation(can::StandardDataFrame("4200#00000000000000")).id() == ID_INVERTER_REQUEST_ENSEMBLE_INFO);
}
