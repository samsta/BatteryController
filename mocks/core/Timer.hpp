#ifndef _MOCKS_CORE_TIMER_HPP
#define _MOCKS_CORE_TIMER_HPP

#include "core/Timer.hpp"
#include "core/USBInterface/USBInterface.hpp"
// forward declaration
namespace logging{
   class Logger;
}

namespace mocks {
namespace core {

class Timer: public ::core::Timer
{
public:
   MOCK_METHOD(void, registerPeriodicCallback, (::core::Invokable* invokable, unsigned period_ms, const char* timer_name));
   MOCK_METHOD(void, schedule, (::core::Invokable* invokable, unsigned delay_ms));
   MOCK_METHOD(void, deregisterCallback, (::core::Invokable* invokable));
   MOCK_METHOD(void, setLogger, (logging::Logger* log));
};

// class TeensyRelay: public ::core::TeensyRelay
// {
// public:
//    MOCK_METHOD(bool, getRelayState, ());
//    MOCK_METHOD(void, setRelayState,(bool newstate));
// };

// class PackSafetyLBC: public ::core::PackSafetyLBC
// {
// public:
//    MOCK_METHOD(void, setSafetyRelayState, (bool newstate));
//    MOCK_METHOD(void, setLBCRelayState, (bool newstate));
//    MOCK_METHOD(bool, getSafetyRelayState, ());
//    MOCK_METHOD(bool, getLBCRelayState, ());
// };

}
}

#endif // _MOCKS_CORE_TIMER_HPP
