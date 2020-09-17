#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/Nissan/PackTemperatures.hpp"
#include <vector>

using namespace can::messages::Nissan;
using namespace testing;
using std::isnan;

namespace {

class DataFrame7bbGroup2: public can::DataFrame
{
public:
   DataFrame7bbGroup2(unsigned id = 0x7bb, unsigned size = 16): DataFrame(id, size), m_data(size)
   {
      // group is at offset 1
      m_data[1] = 0x04;
   }

   virtual const uint8_t* data() const
   {
      return m_data.data();
   }

   virtual uint8_t* data()
   {
      return m_data.data();
   }

   std::vector<uint8_t> m_data;
};

}

TEST(PackTemperatures, valid)
{
   DataFrame7bbGroup2 frame;

   EXPECT_TRUE(PackTemperatures(frame).valid());
}

TEST(PackTemperatures, invalidIfNot7bb)
{
   DataFrame7bbGroup2 frame(0x7cc);

   EXPECT_FALSE(PackTemperatures(frame).valid());
}

TEST(PackTemperatures, invalidIfWrongSize)
{
   DataFrame7bbGroup2 frame(0x7bb, 8);

   EXPECT_FALSE(PackTemperatures(frame).valid());
}

TEST(PackTemperatures, invalidIfNotGroup4)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[1]++;

   EXPECT_FALSE(PackTemperatures(frame).valid());
}

TEST(PackTemperatures, readAllZeroIfAllBytesZero)
{
   DataFrame7bbGroup2 frame;
   PackTemperatures temperatures(frame);

   for(unsigned k = 0; k < 4; k ++)
   {
      SCOPED_TRACE(testing::Message("pack number ") << k);
      EXPECT_EQ(0, temperatures.getTemperature(k));
   }
}

TEST(PackTemperatures, readNaNForCellsBeyondBounds)
{
   DataFrame7bbGroup2 frame;
   PackTemperatures temperatures(frame);

   EXPECT_TRUE(isnan(temperatures.getTemperature(5)));
}

TEST(PackTemperatures, unitIs1DegC)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[4] = 0x01;

   PackTemperatures temperatures(frame);

   EXPECT_EQ(1, temperatures.getTemperature(0));
}

TEST(PackTemperatures, allTemperaturesMaxAndMin)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[4]  = 0x7f;
   frame.m_data[7]  = 0x80;
   frame.m_data[10] = 0x7f;
   frame.m_data[13] = 0x80;

   PackTemperatures temperatures(frame);

   EXPECT_EQ( 127, temperatures.getTemperature(0));
   EXPECT_EQ(-128, temperatures.getTemperature(1));
   EXPECT_EQ( 127, temperatures.getTemperature(2));
   EXPECT_EQ(-128, temperatures.getTemperature(3));
}

TEST(PackTemperatures, temperature2NaNIfAdcAll1s)
{
   DataFrame7bbGroup2 frame;
   frame.m_data[8] = 0xff;
   frame.m_data[9] = 0xff;

   PackTemperatures temperatures(frame);

   EXPECT_FALSE(isnan(temperatures.getTemperature(0)));
   EXPECT_FALSE(isnan(temperatures.getTemperature(1)));
   EXPECT_TRUE(isnan(temperatures.getTemperature(2)));
   EXPECT_FALSE(isnan(temperatures.getTemperature(3)));
}


TEST(PackTemperatures, toString)
{
   std::ostringstream string;
   DataFrame7bbGroup2 frame;
   frame.m_data[4]  = -1;
   frame.m_data[5] = 0xff;
   frame.m_data[6] = 0xff;
   frame.m_data[10] = -3;
   frame.m_data[13] = 4;

   PackTemperatures temperatures(frame);
   string << temperatures;
   EXPECT_EQ(string.str(), "PackTemperatures: -1degC, ---, -3degC, 4degC");
}


TEST(PackTemperatures, invalidToString)
{
   std::ostringstream string;
   PackTemperatures temperatures(DataFrame7bbGroup2(0));
   string << temperatures;
   EXPECT_EQ(string.str(), "PackTemperatures: invalid");
}

