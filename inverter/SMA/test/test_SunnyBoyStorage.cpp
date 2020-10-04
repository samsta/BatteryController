#include <gmock/gmock.h>
#include "inverter/SMA/SunnyBoyStorage.hpp"
#include "mocks/can/FrameSink.hpp"
#include "mocks/core/Timer.hpp"
#include "mocks/monitor/Monitor.hpp"

using namespace testing;

namespace inverter {
namespace SMA {
namespace {

TEST(SunnyBoyStorage, constructs)
{
   mocks::can::FrameSink sink;
   mocks::core::Timer timer;
   mocks::monitor::Monitor monitor;
   SunnyBoyStorage sbs(sink, timer, monitor);
}

}
}
}
