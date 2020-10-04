#ifndef _MOCKS_CORE_TIMER_HPP
#define _MOCKS_CORE_TIMER_HPP

#include "core/Timer.hpp"

namespace mocks {
namespace core {

class Timer: public ::core::Timer
{
   MOCK_METHOD(void, registerPeriodicCallback, (::core::Invokable& invokable, unsigned period_ms));
   MOCK_METHOD(void, deregisterCallback, (::core::Invokable& invokable));
};

}
}

#endif // _MOCKS_CORE_TIMER_HPP
