#include <gmock/gmock.h>
#include "can/DataFrame.hpp"
#include "can/messages/Tesla/DetailedCellData.hpp"
#include <math.h>

typedef std::array<uint8_t, 8> arr8;

can::DataFrame makeFrame(unsigned id, const std::vector<uint8_t>& data)
{
   return can::DataFrame(id, data.data(), data.size());
}

namespace
{
const float VOLT_PER_UNIT = 0.0008;
const float MAX_VOLTAGE = VOLT_PER_UNIT * 0x3FFF;
const float DEGC_PER_UNIT = 0.01;
const float MAX_TEMPERATURE = DEGC_PER_UNIT *  0x1FFF;
const float MIN_TEMPERATURE = -MAX_TEMPERATURE - DEGC_PER_UNIT;
}

using namespace can;
using namespace can::messages::Tesla;

TEST(DetailedCellData, Index0IsVoltage)
{
   DetailedCellData message("6F2#0000000000000000");

   EXPECT_TRUE(message.isVoltage());
   EXPECT_FALSE(message.isTemperature());
}

TEST(DetailedCellData, Index23IsVoltage)
{
   DetailedCellData message("6F2#1700000000000000");

   EXPECT_TRUE(message.isVoltage());
   EXPECT_FALSE(message.isTemperature());
}

TEST(DetailedCellData, Index24IsTemperature)
{
   DetailedCellData message("6F2#1800000000000000");

   EXPECT_TRUE(message.isTemperature());
   EXPECT_FALSE(message.isVoltage());
}

TEST(DetailedCellData, Index0IsBaseIndex0)
{
   DetailedCellData message("6F2#0000000000000000");

   EXPECT_EQ(0, message.getBaseIndex()) << "expected cell 0";
}

TEST(DetailedCellData, Index23IsBaseIndex92)
{
   DetailedCellData message("6F2#1700000000000000");

   EXPECT_EQ(92, message.getBaseIndex()) << "expected cell 92";
}

TEST(DetailedCellData, Index24IsBaseIndex0)
{
   DetailedCellData message("6F2#1800000000000000");

   EXPECT_EQ(0, message.getBaseIndex()) << "expected sensor 0";
}

TEST(DetailedCellData, Index25IsBaseIndex4)
{
   DetailedCellData message("6F2#1900000000000000");

   EXPECT_EQ(4, message.getBaseIndex()) << "expected sensor 4";
}

TEST(DetailedCellData, Index31IsBaseIndex28)
{
   DetailedCellData message("6F2#1F00000000000000");

   EXPECT_EQ(28, message.getBaseIndex()) << "expected sensor 4";
}

TEST(DetailedCellData, allValuesZero)
{
   DetailedCellData message("6F2#0000000000000000");

   EXPECT_EQ(0, message.getValue(0));
   EXPECT_EQ(0, message.getValue(1));
   EXPECT_EQ(0, message.getValue(2));
   EXPECT_EQ(0, message.getValue(3));
}

TEST(DetailedCellData, valueForIndexOutOfBoundReturnsNaN)
{
   DetailedCellData message("6F2#0000000000000000");

   EXPECT_TRUE(std::isnan(message.getValue(4)));
}

TEST(DetailedCellData, Value0AsUnitVoltage)
{
   DetailedCellData message("6F2#0001000000000000");

   EXPECT_FLOAT_EQ(VOLT_PER_UNIT, message.getValue(0));
}

TEST(DetailedCellData, Value0AsMaxVoltage)
{
   DetailedCellData message("6F2#00FF3F0000000000");

   EXPECT_FLOAT_EQ(MAX_VOLTAGE, message.getValue(0));
}

TEST(DetailedCellData, Value0AsMaxTemperature)
{
   DetailedCellData message("6F2#18FF1F0000000000");

   EXPECT_FLOAT_EQ(MAX_TEMPERATURE, message.getValue(0));
}

TEST(DetailedCellData, Value0AsMinTemperature)
{
   DetailedCellData message("6F2#1800200000000000");

   EXPECT_FLOAT_EQ(MIN_TEMPERATURE, message.getValue(0));
}

TEST(DetailedCellData, Id6F2IsValid)
{
   DetailedCellData message("6F2#0000000000000000");

   EXPECT_TRUE(message.valid());
}

TEST(DetailedCellData, Id6F3IsInvalid)
{
   DetailedCellData message("6F3#0000000000000000");

   EXPECT_FALSE(message.valid());
}

TEST(DetailedCellData, Index31IsValid)
{
   DetailedCellData message("6F2#1F00000000000000");

   EXPECT_TRUE(message.valid());
}

TEST(DetailedCellData, Index32IsInvalid)
{
   DetailedCellData message("6F2#2000000000000000");

   EXPECT_FALSE(message.valid());
}



