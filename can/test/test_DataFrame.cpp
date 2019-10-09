#include <gmock/gmock.h>
#include "DataFrame.hpp"
#include <array>

using namespace can;

typedef std::array<uint8_t, 8> arr8;


TEST(DataFrame, accessBytesAsBitField)
{
   DataFrame f(0, arr8({0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}).begin());

   EXPECT_EQ(0x11, f.getBitField(0,  8));
   EXPECT_EQ(0x22, f.getBitField(8,  8));
   EXPECT_EQ(0x88, f.getBitField(56, 8));

   EXPECT_EQ(0x2211, f.getBitField(0, 16));
   EXPECT_EQ(0x3322, f.getBitField(8, 16));
   EXPECT_EQ(0x8877, f.getBitField(48, 16));

   EXPECT_EQ(0x8877665544332211ULL, f.getBitField(0, 64));
}

TEST(DataFrame, accessLessThanByteAsBitField)
{
   DataFrame f(0, arr8({0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}).begin());

   EXPECT_EQ(0x11, f.getBitField(0, 5));
   EXPECT_EQ(0x1, f.getBitField(0, 4));
   EXPECT_EQ(0x0, f.getBitField(1, 3));
   EXPECT_EQ(0x8, f.getBitField(1, 4));

   EXPECT_EQ(0x1, f.getBitField(63, 1));
   EXPECT_EQ(0x2, f.getBitField(62, 2));
}

TEST(DataFrame, accessAcrossByteBoundaries)
{
   DataFrame f(0, arr8({0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}).begin());

   EXPECT_EQ(0x21, f.getBitField(4, 8));
   EXPECT_EQ(0x221, f.getBitField(4, 12));
   EXPECT_EQ(0x3221, f.getBitField(4, 16));
   EXPECT_EQ(0x3221, f.getBitField(4, 14));
   EXPECT_EQ(0x1221, f.getBitField(4, 13));

   EXPECT_EQ(0x655443, f.getBitField(20, 24));
   EXPECT_EQ(0x32aa21, f.getBitField(21, 24));
   EXPECT_EQ(0x995510, f.getBitField(22, 24));
   EXPECT_EQ(0x195510, f.getBitField(22, 23));
   EXPECT_EQ(0x195510, f.getBitField(22, 21));
   EXPECT_EQ(0x95510, f.getBitField(22, 20));
   
   EXPECT_EQ(0x887, f.getBitField(52, 12));
}

TEST(DataFrame, accessSigned)
{
   DataFrame f(0, arr8({0x11, 0x22, 0xf0, 0xff, 0x55, 0x66, 0x77, 0x88}).begin());

   EXPECT_EQ(int8_t(0x11), f.getSignedBitField(0, 8));
   EXPECT_EQ(int8_t(0x88), f.getSignedBitField(56, 8));

   EXPECT_EQ(int8_t(0x77), f.getSignedBitField(48, 8));
   EXPECT_EQ(int8_t(0xee), f.getSignedBitField(47, 8));

   EXPECT_EQ(-1, f.getSignedBitField(0, 1));
   EXPECT_EQ(-1, f.getSignedBitField(48, 3));
   EXPECT_EQ( 7, f.getSignedBitField(48, 4));
   EXPECT_EQ( 3, f.getSignedBitField(49, 3));
   EXPECT_EQ(-5, f.getSignedBitField(49, 4));

   EXPECT_EQ(-1, f.getSignedBitField(20, 12));
   EXPECT_EQ(-2, f.getSignedBitField(19, 12));
   EXPECT_EQ(-4, f.getSignedBitField(18, 12));
   EXPECT_EQ(-8, f.getSignedBitField(17, 12));
}

TEST(DataFrame, limits)
{
   DataFrame f(0, arr8({0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}).begin());

   EXPECT_EQ(0xffffffffffffffffULL, f.getBitField(0, 64));
   EXPECT_EQ(-1, f.getSignedBitField(0, 64));
}
