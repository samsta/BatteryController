#include <gmock/gmock.h>
#include "logging/Hex.hpp"
#include <sstream>

TEST(Hex, formatUnsigned)
{
   std::stringstream ss;
   ss << logging::Hex(0x4567abcd);
   EXPECT_EQ("4567ABCD", ss.str());
}

TEST(Hex, formatUnsignedDoesntPadWith0)
{
   std::stringstream ss;
   ss << logging::Hex(1);
   EXPECT_EQ("1", ss.str());
}

TEST(Hex, formatUnsignedReturnsStreamToPreviousFmtState)
{
   std::stringstream ss;
   std::ios_base::fmtflags flags = ss.flags();
   ss << logging::Hex(1);
   EXPECT_EQ(flags, ss.flags());
}

TEST(Hex, formatByte)
{
   std::stringstream ss;
   ss << logging::HexByte(0x4d);
   EXPECT_EQ("4D", ss.str());
}

TEST(Hex, formatBytePadsWith0)
{
   std::stringstream ss;
   ss << logging::HexByte(0x1);
   EXPECT_EQ("01", ss.str());
}

TEST(Hex, formatByteReturnsStreamToPreviousFmtState)
{
   std::stringstream ss;
   std::ios_base::fmtflags flags = ss.flags();
   ss << logging::HexByte(1);
   EXPECT_EQ(flags, ss.flags());
}
