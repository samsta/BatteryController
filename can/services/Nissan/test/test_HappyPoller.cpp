#include <gmock/gmock.h>
#include "can/services/Nissan/HappyPoller.hpp"
#include "can/FrameSink.hpp"
#include "can/StandardDataFrame.hpp"
#include "mocks/core/Timer.hpp"

using namespace can;
using namespace can::services::Nissan;
using namespace testing;

namespace {

class TestSender: public can::FrameSink
{
public:
   MOCK_METHOD1(sink, void(const DataFrame&));
};


TEST(HappyPoller, registersAndDeregistersPollingCallbackFor1000MillisecondPeriod)
{
   TestSender sender;
   mocks::core::Timer timer;

   {
      core::Invokable* invokable;
      EXPECT_CALL(timer, registerPeriodicCallback(_, 1000)).WillOnce(SaveArg<0>(&invokable));
      HappyPoller poller(sender, timer);
      EXPECT_CALL(timer, deregisterCallback(invokable));
   }
}


class HappyPollerTest: public Test
{
public:
   HappyPollerTest():
      sender(),
      timer(),
      poll_callback(),
      constructor_expectation(timer, &poll_callback),
      poller(sender, timer)
   {
   }

   TestSender         sender;
   NiceMock<mocks::core::Timer> timer;
   core::Invokable*   poll_callback;

   class ConstructorExpectation
      {
      public:
         ConstructorExpectation(mocks::core::Timer& timer, core::Invokable** invokable)
         {
            EXPECT_CALL(timer, registerPeriodicCallback(_, _)).WillOnce(SaveArg<0>(invokable));
         }
   } constructor_expectation;

   HappyPoller poller;
};

MATCHER_P(AsString, str, std::string(negation ? " doesn't equal " : " equals ") + str){
   return testing::internal::CaseInsensitiveStringEquals<std::string>(PrintToString(arg), str);
}

TEST_F(HappyPollerTest, pollInvokesSender)
{
   EXPECT_CALL(sender, sink(_));

   poll_callback->invoke();
}

TEST_F(HappyPollerTest, pollCyclesElevenAMessagesStartingAtNo3)
{
   for (int k : {1, 2})
   {
      SCOPED_TRACE(Message("k=") << k);

      EXPECT_CALL(sender, sink(AsString("11a#4E4004AA80000370")));
      poll_callback->invoke();

      EXPECT_CALL(sender, sink(AsString("11a#4E4004AAC00000BD")));
      poll_callback->invoke();

      EXPECT_CALL(sender, sink(AsString("11a#4E400455000001AB")));
      poll_callback->invoke();

      EXPECT_CALL(sender, sink(AsString("11a#4E40045540000266")));
      poll_callback->invoke();
   }
}

TEST_F(HappyPollerTest, send0xFFResponseToInvalid55B)
{
   EXPECT_CALL(sender, sink(AsString("50c#00000000FFFF")));

   poller.received(StandardDataFrame("55b#0000000000000000"));
}

//TEST_F(HappyPollerTest, doesntRequestSubsubsequentFrameOnReceptionOfSubsequentMessage)
//{
//   EXPECT_CALL(sender, sink(_)).Times(0);
//
//   poller.received(StandardDataFrame("7bb#2100000000000000"));
//}
//
//TEST_F(HappyPollerTest, doesntRequestSubsubsequentFrameOnReceptionOfShortInitialMessage)
//{
//   EXPECT_CALL(sender, sink(_)).Times(0);
//
//   poller.received(StandardDataFrame("7bb#10000000000000"));
//}
//
//TEST_F(HappyPollerTest, doesntRequestSubsubsequentFrameOnReceptionOfWrongIdInitialMessage)
//{
//   EXPECT_CALL(sender, sink(_)).Times(0);
//
//   poller.received(StandardDataFrame("7bc#1000000000000000"));
//}


}
