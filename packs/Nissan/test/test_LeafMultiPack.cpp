#include <gmock/gmock.h>
#include "packs/Nissan/LeafMultiPack.hpp"
#include "mocks/contactor/Contactor.hpp"
#include "mocks/monitor/Monitor.hpp"

using namespace testing;

namespace packs {
namespace Nissan {

class TestLeafMultipack: public Test {
public:
    TestLeafMultipack():
        m_pack(1, &m_monitor1, &m_contactor1, nullptr, nullptr, nullptr, nullptr)
    {
    }

    NiceMock<mocks::monitor::Monitor>     m_monitor1;
    NiceMock<mocks::contactor::Contactor> m_contactor1;
    LeafMultiPack                         m_pack;
};

TEST_F(TestLeafMultipack, isClosedCallsisClosedOnContactor)
{
    EXPECT_CALL(m_contactor1, isClosed());

    m_pack.isClosed();
}

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
