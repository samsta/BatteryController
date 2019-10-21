#include <gmock/gmock.h>
#include "can/services/Nissan/FrameAggregator.hpp"
#include "can/StandardDataFrame.hpp"
#include "can/LargeDataFrame.hpp"

using namespace can;
using namespace can::services::Nissan;
using namespace testing;

namespace can {
void PrintTo(const DataFrame& f, std::ostream* os)
{
   *os << f;
}
}

MATCHER_P(AsString, str, std::string(negation ? " doesn't equal " : " equals ") + str){
   return testing::internal::CaseInsensitiveStringEquals<std::string>(PrintToString(arg), str);
}


class TestSink: public can::FrameSink
{
public:
   MOCK_METHOD1(sink, void(const DataFrame& ));
};

class FrameAggregatorTest: public Test
{
public:
   FrameAggregatorTest(): output(), aggregator(output){}

   TestSink output;
   FrameAggregator aggregator;
};

TEST_F(FrameAggregatorTest, aggregatesTwoLine9ByteMessage)
{
   EXPECT_CALL(output, sink(AsString("7bb#610001020304050607")));

   aggregator.sink(StandardDataFrame("7bb#1009610001020304"));
   aggregator.sink(StandardDataFrame("7bb#21050607ffffffff"));
}

TEST_F(FrameAggregatorTest, aggregatesThreeLine20ByteMessage)
{
   EXPECT_CALL(output, sink(AsString("7bb#61000102030405060708090a0b0c0d0e0f101112")));

   aggregator.sink(StandardDataFrame("7bb#1014610001020304"));
   aggregator.sink(StandardDataFrame("7bb#2105060708090a0b"));
   aggregator.sink(StandardDataFrame("7bb#220c0d0e0f101112"));
}

TEST_F(FrameAggregatorTest, aggregatesOneLine3ByteMessage)
{
   EXPECT_CALL(output, sink(AsString("7bb#610001")));

   aggregator.sink(StandardDataFrame("7bb#1003610001ffffff"));
}

TEST_F(FrameAggregatorTest, aggregatesRepeatedly)
{
   EXPECT_CALL(output, sink(AsString("7bb#610001020304050607")));

   aggregator.sink(StandardDataFrame("7bb#1009610001020304"));
   aggregator.sink(StandardDataFrame("7bb#21050607ffffffff"));

   EXPECT_CALL(output, sink(AsString("7bb#610001")));

   aggregator.sink(StandardDataFrame("7bb#1003610001ffffff"));
}

TEST_F(FrameAggregatorTest, rejectWrongSequenceInitialisation)
{
   EXPECT_CALL(output, sink(_)).Times(0);

   aggregator.sink(StandardDataFrame("7bb#2103610001ffffff"));
   //              this ought to be 10----^^
}

TEST_F(FrameAggregatorTest, rejectWrongSequenceContinuation)
{
   EXPECT_CALL(output, sink(_)).Times(0);

   aggregator.sink(StandardDataFrame("7bb#1014610001020304"));
   aggregator.sink(StandardDataFrame("7bb#2105060708090a0b"));
   aggregator.sink(StandardDataFrame("7bb#230c0d0e0f101112"));
   //              this ought to be 22----^^
}

TEST_F(FrameAggregatorTest, restartsAggregationAfterWrongSequenceContinuation)
{
   EXPECT_CALL(output, sink(_)).Times(0);

   aggregator.sink(StandardDataFrame("7bb#1009610001020304"));
   aggregator.sink(StandardDataFrame("7bb#33050607ffffffff"));
   //             this ought to be 21 ----^^

   EXPECT_CALL(output, sink(AsString("7bb#610001")));

   aggregator.sink(StandardDataFrame("7bb#1003610001ffffff"));
}

TEST_F(FrameAggregatorTest, large118ByteMessageThatWrapsSequenceNumber)
{
   EXPECT_CALL(output, sink(_));

   aggregator.sink(StandardDataFrame("7bb#1076610000000000"));
   //                       118 in hex -----^^
   aggregator.sink(StandardDataFrame("7bb#2100000000000000"));
   aggregator.sink(StandardDataFrame("7bb#2200000000000000"));
   aggregator.sink(StandardDataFrame("7bb#2300000000000000"));
   aggregator.sink(StandardDataFrame("7bb#2400000000000000"));
   aggregator.sink(StandardDataFrame("7bb#2500000000000000"));
   aggregator.sink(StandardDataFrame("7bb#2600000000000000"));
   aggregator.sink(StandardDataFrame("7bb#2700000000000000"));
   aggregator.sink(StandardDataFrame("7bb#2800000000000000"));
   aggregator.sink(StandardDataFrame("7bb#2900000000000000"));
   aggregator.sink(StandardDataFrame("7bb#2a00000000000000"));
   aggregator.sink(StandardDataFrame("7bb#2b00000000000000"));
   aggregator.sink(StandardDataFrame("7bb#2c00000000000000"));
   aggregator.sink(StandardDataFrame("7bb#2d00000000000000"));
   aggregator.sink(StandardDataFrame("7bb#2e00000000000000"));
   aggregator.sink(StandardDataFrame("7bb#2f00000000000000"));
   aggregator.sink(StandardDataFrame("7bb#2000000000000000"));
}
