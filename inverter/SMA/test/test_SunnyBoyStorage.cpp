#include <gmock/gmock.h>
#include "inverter/SMA/SunnyBoyStorage.hpp"
#include "mocks/can/FrameSink.hpp"
#include "mocks/core/Timer.hpp"
#include "mocks/monitor/Monitor.hpp"
#include "core/Callback.hpp"
#include "can/messages/SMA/BatteryState.hpp"

using namespace testing;
using namespace can::messages::SMA;

namespace inverter {
namespace SMA {
namespace {

MATCHER_P(MatchesMessage, m, "")
{
   return arg == m;
}

TEST(SunnyBoyStorage, registersAndDeregistersTimerCallbackFor5000MillisecondPeriod)
{
   mocks::can::FrameSink sink;
   mocks::core::Timer timer;
   mocks::monitor::Monitor monitor;

   {
      core::Invokable* invokable;
      EXPECT_CALL(timer, registerPeriodicCallback(_, 5000)).WillOnce(SaveArg<0>(&invokable));
      SunnyBoyStorage sbs(sink, timer, monitor);
      EXPECT_CALL(timer, deregisterCallback(invokable));
   }
}

class SunnyBoyStorageTest: public Test
{
public:
   SunnyBoyStorageTest():
      constructor_expectation(timer, &broadcast_callback),
      sbs(sink, timer, monitor)
   {
   }

   NiceMock<mocks::can::FrameSink>   sink;
   NiceMock<mocks::core::Timer>      timer;
   NiceMock<mocks::monitor::Monitor> monitor;
   core::Invokable*                  broadcast_callback;

   class ConstructorExpectation
   {
   public:
      ConstructorExpectation(mocks::core::Timer& timer, core::Invokable** invokable)
      {
         EXPECT_CALL(timer, registerPeriodicCallback(_, _)).WillOnce(SaveArg<0>(invokable));
      }
   } constructor_expectation;
   
   SunnyBoyStorage sbs;
};

TEST_F(SunnyBoyStorageTest, publishesBatteryStateUponBroadcast)
{
   EXPECT_CALL(monitor, getSocPercent()).WillOnce(Return(87.6));
   EXPECT_CALL(monitor, getSohPercent()).WillOnce(Return(67.8));
   EXPECT_CALL(monitor, getEnergyRemainingKwh()).WillOnce(Return(123.4));
   EXPECT_CALL(monitor, getFullChargedEnergyKwh()).WillOnce(Return(567.8));

   EXPECT_CALL(sink, sink(MatchesMessage(BatteryState(87.6, 67.8, 123.4, 567.8))));
   
   broadcast_callback->invoke();
}

}
}
}
