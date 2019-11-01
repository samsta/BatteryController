#include <gmock/gmock.h>
#include "can/services/Nissan/GroupPoller.hpp"
#include "can/FrameSink.hpp"
#include "can/StandardDataFrame.hpp"

using namespace can;
using namespace can::services::Nissan;
using namespace testing;

namespace {

class TestSender: public can::FrameSink
{
public:
   MOCK_METHOD1(sink, void(const DataFrame&));
};

class GroupPollerTest: public Test
{
public:
   GroupPollerTest():
      poller(sender)
   {
   }

   TestSender sender;
   GroupPoller poller;
};

MATCHER_P(AsString, str, std::string(negation ? " doesn't equal " : " equals ") + str){
   return testing::internal::CaseInsensitiveStringEquals<std::string>(PrintToString(arg), str);
}

TEST_F(GroupPollerTest, pollInvokesSender)
{
   EXPECT_CALL(sender, sink(_));

   poller.poll();
}

TEST_F(GroupPollerTest, pollCyclesThroughGroupsAndRequestsInitialFrame)
{
   for (int k : {1, 2})
   {
      SCOPED_TRACE(Message("k=") << k);

      EXPECT_CALL(sender, sink(AsString("79b#0221010000000000")));
      poller.poll();

      EXPECT_CALL(sender, sink(AsString("79b#0221020000000000")));
      poller.poll();

      EXPECT_CALL(sender, sink(AsString("79b#0221040000000000")));
      poller.poll();
   }
}

TEST_F(GroupPollerTest, requestsSubsubsequentFrameOnReceptionOfInitialMessage)
{
   EXPECT_CALL(sender, sink(AsString("79b#3000000000000000")));

   poller.received(StandardDataFrame("7bb#1000000000000000"));
}

TEST_F(GroupPollerTest, doesntRequestSubsubsequentFrameOnReceptionOfSubsequentMessage)
{
   EXPECT_CALL(sender, sink(_)).Times(0);

   poller.received(StandardDataFrame("7bb#2100000000000000"));
}

TEST_F(GroupPollerTest, doesntRequestSubsubsequentFrameOnReceptionOfShortInitialMessage)
{
   EXPECT_CALL(sender, sink(_)).Times(0);

   poller.received(StandardDataFrame("7bb#10000000000000"));
}

TEST_F(GroupPollerTest, doesntRequestSubsubsequentFrameOnReceptionOfWrongIdInitialMessage)
{
   EXPECT_CALL(sender, sink(_)).Times(0);

   poller.received(StandardDataFrame("7bc#1000000000000000"));
}


}
