#include <gmock/gmock.h>
#include "core/Callback.hpp"
#include "contactor/Nissan/LeafContactor.hpp"
#include "mocks/core/OutputPin.hpp"
#include "mocks/core/Timer.hpp"
#include "can/FrameSink.hpp"
#include "can/StandardDataFrame.hpp"

using namespace testing;
using namespace can;

namespace contactor {
namespace Nissan {
namespace {

TEST(NissanLeafContactor, setsOutputPinsToDefault)
{
   mocks::core::Timer timer;
   mocks::core::OutputPin positive_relay;
   mocks::core::OutputPin negative_relay;
   mocks::core::OutputPin indicator_led;

   EXPECT_CALL(positive_relay, set(mocks::core::OutputPin::HIGH));
   EXPECT_CALL(negative_relay, set(mocks::core::OutputPin::HIGH));
   EXPECT_CALL(indicator_led, set(mocks::core::OutputPin::LOW));
   
   LeafContactor contactor(timer,
                           positive_relay,
                           negative_relay,
                           indicator_led,
                           &std::cout);

   // expect contactor to open upon destruction
   EXPECT_CALL(positive_relay, set(mocks::core::OutputPin::HIGH));
   EXPECT_CALL(negative_relay, set(mocks::core::OutputPin::HIGH));
   EXPECT_CALL(indicator_led, set(mocks::core::OutputPin::LOW));
}

class NissanLeafContactorTest: public Test
{
public:

   NissanLeafContactorTest():
      timer(),
      positive_relay(),
      negative_relay(),
      indicator_led(),
      contactor(timer,
                positive_relay,
                negative_relay,
                indicator_led,
                &std::cout)
   {
   }

   ~NissanLeafContactorTest()
   {
      // expect contactor to open upon destruction
      EXPECT_CALL(positive_relay, set(mocks::core::OutputPin::HIGH));
      EXPECT_CALL(negative_relay, set(mocks::core::OutputPin::HIGH));
      EXPECT_CALL(indicator_led, set(mocks::core::OutputPin::LOW));
   }

   NiceMock<mocks::core::Timer> timer;
   NiceMock<mocks::core::OutputPin> positive_relay;
   NiceMock<mocks::core::OutputPin> negative_relay;
   NiceMock<mocks::core::OutputPin> indicator_led;
   LeafContactor contactor;
};

TEST_F(NissanLeafContactorTest, openedInitially)
{
   EXPECT_FALSE(contactor.isClosed());
}

TEST_F(NissanLeafContactorTest, dontCloseIfUnsafe)
{
   contactor.setSafeToOperate(false);

   EXPECT_CALL(positive_relay, set(_)).Times(0);
   EXPECT_CALL(negative_relay, set(_)).Times(0);
   EXPECT_CALL(indicator_led, set(_)).Times(0);

   contactor.close();

   EXPECT_FALSE(contactor.isClosed());
}

TEST_F(NissanLeafContactorTest, dontScheduleTimerIfUnsafe)
{
   contactor.setSafeToOperate(false);

   EXPECT_CALL(timer, schedule(_, _)).Times(0);

   contactor.close();
}

TEST_F(NissanLeafContactorTest, setNegativeRelayAndIndicatorOnCloseIfSafe)
{
   contactor.setSafeToOperate(true);

   EXPECT_CALL(negative_relay, set(mocks::core::OutputPin::LOW));
   EXPECT_CALL(indicator_led, set(mocks::core::OutputPin::HIGH));
   EXPECT_CALL(positive_relay, set(_)).Times(0);

   contactor.close();
}

TEST_F(NissanLeafContactorTest, scheduleTimerToClosePositiveRelayOnCloseIfSafe)
{
   contactor.setSafeToOperate(true);

   EXPECT_CALL(timer, schedule(_, 3000));

   contactor.close();
}

TEST_F(NissanLeafContactorTest, stillOpenIfCloseInitiatedButTimerNotElapsed)
{
   contactor.setSafeToOperate(true);
   contactor.close();

   EXPECT_FALSE(contactor.isClosed());
}

TEST_F(NissanLeafContactorTest, closePositiveRelayWhenTimerElapsesAndScheduledActionInvoked)
{
   contactor.setSafeToOperate(true);
   core::Invokable* scheduled_action;
   EXPECT_CALL(timer, schedule(_, 3000)).WillOnce(SaveArg<0>(&scheduled_action));
   contactor.close();

   EXPECT_CALL(positive_relay, set(mocks::core::OutputPin::LOW));
   EXPECT_CALL(negative_relay, set(_)).Times(0);
   EXPECT_CALL(indicator_led, set(_)).Times(0);

   scheduled_action->invoke();
}

class NissanLeafContactorClosedTest: public NissanLeafContactorTest
{
public:
   NissanLeafContactorClosedTest()
   {
      contactor.setSafeToOperate(true);
      core::Invokable* scheduled_action;
      EXPECT_CALL(timer, schedule(_, 3000)).WillOnce(SaveArg<0>(&scheduled_action));
      contactor.close();
      scheduled_action->invoke();
   }
};

TEST_F(NissanLeafContactorClosedTest, contactorClosed)
{
   EXPECT_TRUE(contactor.isClosed());
}

TEST_F(NissanLeafContactorClosedTest, noActionIfClosedAgain)
{
   EXPECT_CALL(positive_relay, set(_)).Times(0);
   EXPECT_CALL(negative_relay, set(_)).Times(0);
   EXPECT_CALL(indicator_led, set(_)).Times(0);

   contactor.close();

   EXPECT_TRUE(contactor.isClosed());
}

TEST_F(NissanLeafContactorClosedTest, opensIfOpened)
{
   EXPECT_CALL(positive_relay, set(mocks::core::OutputPin::HIGH));
   EXPECT_CALL(negative_relay, set(mocks::core::OutputPin::HIGH));
   EXPECT_CALL(indicator_led, set(mocks::core::OutputPin::LOW));

   contactor.open();

   EXPECT_FALSE(contactor.isClosed());
}

TEST_F(NissanLeafContactorClosedTest, opensIfUnsafe)
{
   EXPECT_CALL(positive_relay, set(mocks::core::OutputPin::HIGH));
   EXPECT_CALL(negative_relay, set(mocks::core::OutputPin::HIGH));
   EXPECT_CALL(indicator_led, set(mocks::core::OutputPin::LOW));

   contactor.setSafeToOperate(false);

   EXPECT_FALSE(contactor.isClosed());
}

//---------------------------------------------------------------------------------------------------
class TestSender: public can::FrameSink
{
public:
   MOCK_METHOD1(sink, void(const DataFrame&));
};

// class LeafSafetyRelayTest: public Test
// {
// public:
//    LeafSafetyRelayTest():
//        sender() // THIS DOES NOT WORK!?!?!?!?!?!?!?!!?
//    {
//    }

//    TestSender        sender;
//    LeafSafetyRelay   contactor;  
// };

// MATCHER_P(AsString, str, std::string(negation ? " doesn't equal " : " equals ") + str){
//    return testing::internal::CaseInsensitiveStringEquals<std::string>(PrintToString(arg), str);
// }

// TEST_F(LeafSafetyRelayTest, initiallyOpensRelay)
// {
//    EXPECT_CALL(sender, sink(AsString("800#5555000000000000")));

//    contactor.setSafeToOperate(false);
   
// }

MATCHER_P(AsString, str, std::string(negation ? " doesn't equal " : " equals ") + str){
   return testing::internal::CaseInsensitiveStringEquals<std::string>(PrintToString(arg), str);
}

TEST(LeafSafetyRelayT, openRelayWhenUnsafe)
{
   TestSender sender;
   LeafSafetyRelay leafsafetyrelay(sender);

   // relay should open when unsafe
   EXPECT_CALL(sender, sink(AsString("800#AAAA000000000000")));

   leafsafetyrelay.setSafeToOperate(false);
}

TEST(LeafSafetyRelayT, closeRelayWhenSafe)
{
   TestSender sender;
   LeafSafetyRelay leafsafetyrelay(sender);

   // relay should close when safe
   EXPECT_CALL(sender, sink(AsString("800#5555000000000000")));

   leafsafetyrelay.setSafeToOperate(true);
}

}
}
}
