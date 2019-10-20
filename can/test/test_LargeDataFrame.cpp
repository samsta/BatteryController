#include <gmock/gmock.h>
#include "LargeDataFrame.hpp"

using namespace can;
using namespace testing;

namespace {
const uint32_t ARBITRARY_ID = 0x321;
}

TEST(LargeDataFrame, invalidWithoutData)
{
   EXPECT_FALSE(LargeDataFrame(ARBITRARY_ID, 2).valid());
}

TEST(LargeDataFrame, validWithData)
{
   uint8_t data[2] = {0, 0};
   EXPECT_TRUE(LargeDataFrame(ARBITRARY_ID, data, 2).valid());
}

TEST(LargeDataFrame, canAccessData)
{
   uint8_t data[] = {1, 2, 3};
   LargeDataFrame frame(ARBITRARY_ID, data, sizeof(data));

   EXPECT_THAT(frame, ElementsAreArray(data, sizeof(data)));
}

TEST(LargeDataFrame, canAppendData)
{
   uint8_t data[] = {4, 5, 6};
   LargeDataFrame frame(ARBITRARY_ID, sizeof(data));

   frame.append(data, sizeof(data));

   ASSERT_TRUE(frame.valid());
   EXPECT_THAT(frame, ElementsAreArray(data, sizeof(data)));
}

TEST(LargeDataFrame, invalidIfNotAppendedToFullSize)
{
   uint8_t data[] = {4, 5, 6};
   LargeDataFrame frame(ARBITRARY_ID, sizeof(data));

   frame.append(data, sizeof(data) - 1);

   ASSERT_FALSE(frame.valid());
}

TEST(LargeDataFrame, validIfAppendedToFullSizeInMoreThanOneSteps)
{
   uint8_t data[] = {4, 5, 6};
   LargeDataFrame frame(ARBITRARY_ID, sizeof(data));

   frame.append(data, sizeof(data) - 1);
   frame.append(data + 2, 1);

   ASSERT_TRUE(frame.valid());
   EXPECT_THAT(frame, ElementsAreArray(data, sizeof(data)));
}

TEST(LargeDataFrame, truncatesOversizeDataInAppend)
{
   uint8_t data[] = {4, 5, 6, 7};
   LargeDataFrame frame(ARBITRARY_ID, sizeof(data) - 1);

   frame.append(data, sizeof(data));

   ASSERT_TRUE(frame.valid());
   EXPECT_THAT(frame, ElementsAreArray(data, sizeof(data) - 1));
}
