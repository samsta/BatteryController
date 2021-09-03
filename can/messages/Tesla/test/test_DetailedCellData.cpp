#include <gmock/gmock.h>
#include "can/StandardDataFrame.hpp"
#include "can/messages/Tesla/DetailedCellData.hpp"
#include <math.h>

namespace
{
const float VOLT_PER_UNIT = 0.0008;
const float MAX_VOLTAGE = VOLT_PER_UNIT * 0x1FFF;
const float DEGC_PER_UNIT = 0.01;
const float MAX_TEMPERATURE = DEGC_PER_UNIT *  0x0FFF;
const float MIN_TEMPERATURE = -MAX_TEMPERATURE - DEGC_PER_UNIT;
}

using namespace can;
using namespace can::messages::Tesla;

std::string toString(const DetailedCellData& d)
{
   std::ostringstream out;
   out << d;
   return out.str();
}

TEST(DetailedCellData, Index0IsVoltage)
{
   DetailedCellData message(StandardDataFrame("6F2#0000000000000000"));

   EXPECT_TRUE(message.isVoltage());
   EXPECT_FALSE(message.isTemperature());
}

TEST(DetailedCellData, Index23IsVoltage)
{
   DetailedCellData message(StandardDataFrame("6F2#1700000000000000"));

   EXPECT_TRUE(message.isVoltage());
   EXPECT_FALSE(message.isTemperature());
}

TEST(DetailedCellData, Index24IsTemperature)
{
   DetailedCellData message(StandardDataFrame("6F2#1800000000000000"));

   EXPECT_TRUE(message.isTemperature());
   EXPECT_FALSE(message.isVoltage());
}

TEST(DetailedCellData, Index0IsBaseIndex0)
{
   DetailedCellData message(StandardDataFrame("6F2#0000000000000000"));

   EXPECT_EQ(0, message.getBaseIndex()) << "expected cell 0";
}

TEST(DetailedCellData, Index23IsBaseIndex92)
{
   DetailedCellData message(StandardDataFrame("6F2#1700000000000000"));

   EXPECT_EQ(92, message.getBaseIndex()) << "expected cell 92";
}

TEST(DetailedCellData, Index24IsBaseIndex0)
{
   DetailedCellData message(StandardDataFrame("6F2#1800000000000000"));

   EXPECT_EQ(0, message.getBaseIndex()) << "expected sensor 0";
}

TEST(DetailedCellData, Index25IsBaseIndex4)
{
   DetailedCellData message(StandardDataFrame("6F2#1900000000000000"));

   EXPECT_EQ(4, message.getBaseIndex()) << "expected sensor 4";
}

TEST(DetailedCellData, Index31IsBaseIndex28)
{
   DetailedCellData message(StandardDataFrame("6F2#1F00000000000000"));

   EXPECT_EQ(28, message.getBaseIndex()) << "expected sensor 4";
}

TEST(DetailedCellData, allValuesZero)
{
   DetailedCellData message(StandardDataFrame("6F2#0000000000000000"));

   EXPECT_EQ(0, message.getValue(0));
   EXPECT_EQ(0, message.getValue(1));
   EXPECT_EQ(0, message.getValue(2));
   EXPECT_EQ(0, message.getValue(3));
}

TEST(DetailedCellData, valueForIndexOutOfBoundReturnsNaN)
{
   DetailedCellData message(StandardDataFrame("6F2#0000000000000000"));

   EXPECT_TRUE(isnan(message.getValue(4)));
}

TEST(DetailedCellData, Value0AsUnitVoltage)
{
   DetailedCellData message(StandardDataFrame("6F2#0001000000000000"));

   EXPECT_FLOAT_EQ(VOLT_PER_UNIT, message.getValue(0));
}

TEST(DetailedCellData, Value0AsMaxVoltage)
{
   DetailedCellData message(StandardDataFrame("6F2#00FF1F0000000000"));

   EXPECT_FLOAT_EQ(MAX_VOLTAGE, message.getValue(0));
}

TEST(DetailedCellData, Value0AsMaxTemperature)
{
   DetailedCellData message(StandardDataFrame("6F2#18FF0F0000000000"));

   EXPECT_FLOAT_EQ(MAX_TEMPERATURE, message.getValue(0));
}

TEST(DetailedCellData, Value0AsMinTemperature)
{
   DetailedCellData message(StandardDataFrame("6F2#1800100000000000"));

   EXPECT_FLOAT_EQ(MIN_TEMPERATURE, message.getValue(0));
}

TEST(DetailedCellData, Value3AsUnitVoltage)
{
   DetailedCellData message(StandardDataFrame("6F2#0000000000000400"));

   EXPECT_FLOAT_EQ(VOLT_PER_UNIT, message.getValue(3));
}

TEST(DetailedCellData, Value3AsMaxVoltage)
{
   DetailedCellData message(StandardDataFrame("6F2#000000000000FC7F"));

   EXPECT_FLOAT_EQ(MAX_VOLTAGE, message.getValue(3));
}

TEST(DetailedCellData, Value3AsMaxTemperature)
{
   DetailedCellData message(StandardDataFrame("6F2#180000000000FC3F"));

   EXPECT_FLOAT_EQ(MAX_TEMPERATURE, message.getValue(3));
}

TEST(DetailedCellData, Value3AsMinTemperature)
{
   DetailedCellData message(StandardDataFrame("6F2#1800100000000040"));

   EXPECT_FLOAT_EQ(MIN_TEMPERATURE, message.getValue(3));
}

TEST(DetailedCellData, Id6F2IsValid)
{
   DetailedCellData message(StandardDataFrame("6F2#0000000000000000"));

   EXPECT_TRUE(message.valid());
}

TEST(DetailedCellData, Id6F3IsInvalid)
{
   DetailedCellData message(StandardDataFrame("6F3#0000000000000000"));

   EXPECT_FALSE(message.valid());
}

TEST(DetailedCellData, Index31IsValid)
{
   DetailedCellData message(StandardDataFrame("6F2#1F00000000000000"));

   EXPECT_TRUE(message.valid());
}

TEST(DetailedCellData, Index32IsInvalid)
{
   DetailedCellData message(StandardDataFrame("6F2#2000000000000000"));

   EXPECT_FALSE(message.valid());
}

TEST(DetailedCellData, ShortMessageIsInvalid)
{
   DetailedCellData message(StandardDataFrame("6F2#00000000000000"));

   EXPECT_FALSE(message.valid());
}

TEST(DetailedCellData, LongMessageIsInvalid)
{
   DetailedCellData message(StandardDataFrame("6F2#000000000000000000"));

   EXPECT_FALSE(message.valid());
}

TEST(DetailedCellData, ToString)
{
   EXPECT_EQ("DetailedCellData: invalid", toString(DetailedCellData()));
   EXPECT_EQ("DetailedCellData: U0=0V, U1=0V, U2=0V, U3=0V", toString(DetailedCellData().setIsVoltage()));
   EXPECT_EQ("DetailedCellData: T0=0degC, T1=0degC, T2=0degC, T3=0degC", toString(DetailedCellData().setIsTemperature()));
   EXPECT_EQ("DetailedCellData: U64=0.03V, U65=1.456V, U66=5V, U67=9.001V", toString(DetailedCellData()
                                                                           .setIsVoltage()
                                                                           .setBaseIndex(64)
                                                                           .setValue(0, 0.03)
                                                                           .setValue(1, 1.456)
                                                                           .setValue(2, 5.0)
                                                                           .setValue(3, 9.001)));

}

