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
