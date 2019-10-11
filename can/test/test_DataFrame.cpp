#include <gmock/gmock.h>
#include "DataFrame.hpp"
#include <array>

using namespace can;

typedef std::array<uint8_t, 8> arr8;

TEST(DataFrame, Construct1ByteFromString)
{
   DataFrame f("abc#de");

   ASSERT_TRUE(f.valid());
   EXPECT_EQ(0xabc, f.id());

   ASSERT_EQ(1, f.size());
   EXPECT_EQ(0xde, f.data()[0]);
}

TEST(DataFrame, Construct2BytesFromString)
{
   DataFrame f("123#4567");

   ASSERT_TRUE(f.valid());
   EXPECT_EQ(0x123, f.id());

   ASSERT_EQ(2, f.size());
   EXPECT_EQ(0x45, f.data()[0]);
   EXPECT_EQ(0x67, f.data()[1]);
}

TEST(DataFrame, Construct8BytesFromStringUpperAndLowerCase)
{
   DataFrame f("12345678#0123456789AbCdEf");

   ASSERT_TRUE(f.valid());
   EXPECT_EQ(0x12345678, f.id());

   ASSERT_EQ(8, f.size());
   EXPECT_EQ(0x01, f.data()[0]);
   EXPECT_EQ(0x23, f.data()[1]);
   EXPECT_EQ(0x45, f.data()[2]);
   EXPECT_EQ(0x67, f.data()[3]);
   EXPECT_EQ(0x89, f.data()[4]);
   EXPECT_EQ(0xAB, f.data()[5]);
   EXPECT_EQ(0xCD, f.data()[6]);
   EXPECT_EQ(0xEF, f.data()[7]);
}

TEST(DataFrame, Construct8BytesFromOversizeString)
{
   DataFrame f("12345678#0123456789AbCdEf1234");

   ASSERT_TRUE(f.valid());
   EXPECT_EQ(0x12345678, f.id());

   ASSERT_EQ(8, f.size());
   EXPECT_EQ(0x01, f.data()[0]);
   EXPECT_EQ(0x23, f.data()[1]);
   EXPECT_EQ(0x45, f.data()[2]);
   EXPECT_EQ(0x67, f.data()[3]);
   EXPECT_EQ(0x89, f.data()[4]);
   EXPECT_EQ(0xAB, f.data()[5]);
   EXPECT_EQ(0xCD, f.data()[6]);
   EXPECT_EQ(0xEF, f.data()[7]);
}

TEST(DataFrame, ZeroLengthFrameFromString)
{
   DataFrame f("123#");

   ASSERT_TRUE(f.valid());
   EXPECT_EQ(0x123, f.id());
   EXPECT_EQ(0, f.size());
}

TEST(DataFrame, NoHashInvalid)
{
   DataFrame f("123x");

   ASSERT_FALSE(f.valid());
}

TEST(DataFrame, NullStringInvalidAndWontCrash)
{
   DataFrame f(NULL);

   ASSERT_FALSE(f.valid());
}

TEST(DataFrame, Construct1Byte)
{
   uint8_t data[] = {0xAB};
   DataFrame f(1234, data, sizeof(data));

   ASSERT_TRUE(f.valid());
   EXPECT_EQ(1234, f.id());

   ASSERT_EQ(1, f.size());
   EXPECT_EQ(0xAB, f.data()[0]);
}

TEST(DataFrame, Construct8BytesFromOversizeArray)
{
   uint8_t data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
   DataFrame f(1234, data, sizeof(data));

   ASSERT_TRUE(f.valid());
   EXPECT_EQ(1234, f.id());

   ASSERT_EQ(8, f.size());
   EXPECT_EQ(1, f.data()[0]);
   EXPECT_EQ(2, f.data()[1]);
   EXPECT_EQ(3, f.data()[2]);
   EXPECT_EQ(4, f.data()[3]);
   EXPECT_EQ(5, f.data()[4]);
   EXPECT_EQ(6, f.data()[5]);
   EXPECT_EQ(7, f.data()[6]);
   EXPECT_EQ(8, f.data()[7]);
}

TEST(DataFrame, NullPtrValidZeroLengthDespiteNonZeroLengthSpecified)
{
   DataFrame f(1234, NULL, 8);

   ASSERT_TRUE(f.valid());
   EXPECT_EQ(0, f.size());
}

TEST(DataFrame, accessBytesAsBitField)
{
   DataFrame f("0#1122334455667788");

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
   DataFrame f("0#1122334455667788");

   EXPECT_EQ(0x11, f.getBitField(0, 5));
   EXPECT_EQ(0x1, f.getBitField(0, 4));
   EXPECT_EQ(0x0, f.getBitField(1, 3));
   EXPECT_EQ(0x8, f.getBitField(1, 4));

   EXPECT_EQ(0x1, f.getBitField(63, 1));
   EXPECT_EQ(0x2, f.getBitField(62, 2));
}

TEST(DataFrame, accessAcrossByteBoundaries)
{
   DataFrame f("0#1122334455667788");

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
   DataFrame f("0#1122f0ff55667788");

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
   DataFrame f("0#ffffffffffffffff");

   EXPECT_EQ(0xffffffffffffffffULL, f.getBitField(0, 64));
   EXPECT_EQ(-1, f.getSignedBitField(0, 64));
}
