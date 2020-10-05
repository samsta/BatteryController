#include <gmock/gmock.h>
#include "inverter/SMA/SunnyBoyStorage.hpp"
#include "mocks/can/FrameSink.hpp"
#include "mocks/core/Timer.hpp"
#include "mocks/monitor/Monitor.hpp"
#include "core/Callback.hpp"

using namespace testing;

namespace inverter {
namespace SMA {
namespace {

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

}
}
}
