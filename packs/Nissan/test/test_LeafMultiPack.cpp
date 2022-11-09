#include <gmock/gmock.h>
#include "packs/Nissan/LeafMultiPack.hpp"
#include "mocks/contactor/Contactor.hpp"
#include "mocks/monitor/Monitor.hpp"
#include "mocks/core/OutputPin.hpp"
#include "mocks/core/Timer.hpp"
#include "logging/colors.hpp"

using namespace testing;

namespace packs {
namespace Nissan {

TEST(TestLeafMultipack1, setsOutputPinsToDefault)
{
   NiceMock<mocks::monitor::Monitor>     m_monitor1;
   NiceMock<mocks::contactor::Contactor> m_contactor1;
   mocks::core::Timer timer;
   mocks::core::OutputPin positive_relay;
   mocks::core::OutputPin negative_relay;
   mocks::core::OutputPin indicator_led;
   std::vector<monitor::Monitor*>         m_vmonitor = {&m_monitor1};
   std::vector<contactor::Contactor*>     m_vcontactor = {&m_contactor1};

   EXPECT_CALL(positive_relay, set(mocks::core::OutputPin::HIGH));
   EXPECT_CALL(negative_relay, set(mocks::core::OutputPin::HIGH));
   EXPECT_CALL(indicator_led, set(mocks::core::OutputPin::LOW));
   
   LeafMultiPack LMP(      m_vmonitor,
                           m_vcontactor,
                           timer,
                           positive_relay,
                           negative_relay,
                           indicator_led,
                           nullptr);

   // expect contactor to open upon destruction
   EXPECT_CALL(positive_relay, set(mocks::core::OutputPin::HIGH));
   EXPECT_CALL(negative_relay, set(mocks::core::OutputPin::HIGH));
   EXPECT_CALL(indicator_led, set(mocks::core::OutputPin::LOW));
}




// class TestLeafMultipack1: public Test {
// public:
//    TestLeafMultipack1():
//       m_multipack(m_vmonitor,
//                   m_vcontactor,
//                   timer,
//                   positive_relay,
//                   negative_relay,
//                   indicator_led,
//                   nullptr)
//    {
//    }

//    NiceMock<mocks::monitor::Monitor>     m_monitor1;
//    NiceMock<mocks::contactor::Contactor> m_contactor1;
//    mocks::core::Timer timer;
//    mocks::core::OutputPin positive_relay;
//    mocks::core::OutputPin negative_relay;
//    mocks::core::OutputPin indicator_led;

//    std::vector<monitor::Monitor*>         m_vmonitor = {&m_monitor1};
//    std::vector<contactor::Contactor*>     m_vcontactor = {&m_contactor1};

//     LeafMultiPack                         m_multipack;
// };

// TEST_F(TestLeafMultipack1, isClosedCallsisClosedOn1Contactor)
// {
//     EXPECT_CALL(m_contactor1, isClosed());

//     EXPECT_FALSE(m_multipack.getMainContactor().isClosed());
// }

















//  class TestLeafMultipack3: public Test {
//  public:
//      TestLeafMultipack3():
//          m_multipack(m_vmonitor, nullptr)
//      {
//      }

//      NiceMock<mocks::monitor::Monitor>     m_monitor1;
//    //   NiceMock<mocks::contactor::Contactor> m_contactor1;
//      NiceMock<mocks::monitor::Monitor>     m_monitor2;
//    //   NiceMock<mocks::contactor::Contactor> m_contactor2;
//      NiceMock<mocks::monitor::Monitor>     m_monitor3;
//    //   NiceMock<mocks::contactor::Contactor> m_contactor3;

//     std::vector<monitor::Monitor*>         m_vmonitor = {&m_monitor1, &m_monitor2, &m_monitor3};
//    //  std::vector<contactor::Contactor*>     m_vcontactor = {&m_contactor1, &m_contactor2, &m_contactor3};

//      LeafMultiPack                         m_multipack;
//  };

// //  TEST_F(TestLeafMultipack3, isClosedCallsisClosedOn3Contactors)
// //  {
// //      EXPECT_CALL(m_contactor1, isClosed());
// //      EXPECT_CALL(m_contactor2, isClosed());
// //      EXPECT_CALL(m_contactor3, isClosed());

// //      EXPECT_FALSE(m_multipack.isClosed());
// //  }

}
}



////////////////////////  example vector passing and iterating

unsigned sumOfVector(const std::vector<unsigned>& vec)
{
    unsigned sum = 0;

    for (const unsigned& value: vec)
    {
        sum += value;
    }
    return sum;
}

// or, alternatively
unsigned sumOfVector2(const std::vector<unsigned>& vec)
{
    unsigned sum = 0;
    
    for (unsigned i = 0; i < vec.size(); i++)
    {
        sum += vec[i];
    }
    return sum;
}


TEST(Vector,sum1) {
    EXPECT_EQ(0, sumOfVector({}));
    EXPECT_EQ(0, sumOfVector2({}));
}

TEST(Vector,sum2) {
    EXPECT_EQ(5, sumOfVector({2, 3}));
    EXPECT_EQ(5, sumOfVector2({2, 3}));
}

TEST(Vector,sum3) {
    EXPECT_EQ(20, sumOfVector({4, 3, 2, 1, 0, 10}));
    EXPECT_EQ(20, sumOfVector2({4, 3, 2, 1, 0, 10}));
}
