#include <gmock/gmock.h>
#include "can/StandardDataFrame.hpp"
#include "can/messages/SMA/InverterCommand.hpp"
#include <vector>

using namespace can::messages::SMA;
using namespace testing;


TEST(SmaInverterCommand, valid)
{
   EXPECT_TRUE(InverterCommand(can::StandardDataFrame("360#0000000000000000")).valid());
}

TEST(SmaInverterCommand, invalidId)
{
   EXPECT_FALSE(InverterCommand(can::StandardDataFrame("361#0000000000000000")).valid());
}

TEST(SmaInverterCommand, invalidLength)
{
   EXPECT_FALSE(InverterCommand(can::StandardDataFrame("360#00000000000000")).valid());
}

TEST(SmaInverterCommand, commandConnect)
{
   EXPECT_EQ(InverterCommand::CONNECT,
             InverterCommand(can::StandardDataFrame("360#0000000000000100")).getCommand());
}

TEST(SmaInverterCommand, commandDisconnect)
{
   EXPECT_EQ(InverterCommand::DISCONNECT,
             InverterCommand(can::StandardDataFrame("360#0000000000000000")).getCommand());
}

TEST(SmaInverterCommand, otherCommandsInterpretedAsDisconnect)
{
   EXPECT_EQ(InverterCommand::DISCONNECT,
             InverterCommand(can::StandardDataFrame("360#000000000000FF00")).getCommand());
   EXPECT_EQ(InverterCommand::DISCONNECT,
             InverterCommand(can::StandardDataFrame("360#0000000000000200")).getCommand());
}

TEST(SmaInverterCommand, voltageUnitIs0V1)
{
   EXPECT_FLOAT_EQ(0.1,
                   InverterCommand(can::StandardDataFrame("360#0001000000000000")).getMeasuredBatteryVoltage());
}

TEST(SmaInverterCommand, voltageMaximumIs65535V6)
{
   EXPECT_FLOAT_EQ(6553.5,
                   InverterCommand(can::StandardDataFrame("360#FFFF000000000000")).getMeasuredBatteryVoltage());
}

TEST(SmaInverterCommand, currentUnitIs0A1)
{
   EXPECT_FLOAT_EQ(0.1,
                   InverterCommand(can::StandardDataFrame("360#0000000100000000")).getMeasuredBatteryCurrent());
}

TEST(SmaInverterCommand, currentMaxIs3276A7)
{
   EXPECT_FLOAT_EQ(3276.7,
                   InverterCommand(can::StandardDataFrame("360#00007FFF00000000")).getMeasuredBatteryCurrent());
}

TEST(SmaInverterCommand, currentMinIsNeg3276A8)
{
   EXPECT_FLOAT_EQ(-3276.8,
                   InverterCommand(can::StandardDataFrame("360#0000800000000000")).getMeasuredBatteryCurrent());
}

TEST(SmaInverterCommand, temperatureUnitIs0C1)
{
   EXPECT_FLOAT_EQ(0.1,
                   InverterCommand(can::StandardDataFrame("360#0000000000010000")).getMeasuredBatteryTemperature());
}

TEST(SmaInverterCommand, temperatureMaxIs3276C7)
{
   EXPECT_FLOAT_EQ(3276.7,
                   InverterCommand(can::StandardDataFrame("360#000000007FFF0000")).getMeasuredBatteryTemperature());
}

TEST(SmaInverterCommand, temperatureMinIsNeg3276C8)
{
   EXPECT_FLOAT_EQ(-3276.8,
                   InverterCommand(can::StandardDataFrame("360#0000000080000000")).getMeasuredBatteryTemperature());
}

TEST(SmaInverterCommand, invalidToString)
{
   InverterCommand command (can::StandardDataFrame("361#00"));
   std::ostringstream string;
   string << command;
   EXPECT_EQ(string.str(), "InverterCommand: invalid");
}

TEST(SmaInverterCommand, validConnectToString)
{
   InverterCommand command(can::StandardDataFrame("360#0D80FB2Eff850100"));
   std::ostringstream string;
   string << command;
   EXPECT_EQ(string.str(), "InverterCommand: Command=CONNECT MeasuredBatteryVoltage=345.6V MeasuredBatteryCurrent=-123.4A MeasuredBatteryTemperature=-12.3degC");
}

TEST(SmaInverterCommand, validDisconnectToString)
{
   InverterCommand command(can::StandardDataFrame("360#0D80FB2Eff850000"));
   std::ostringstream string;
   string << command;
   EXPECT_EQ(string.str(), "InverterCommand: Command=DISCONNECT MeasuredBatteryVoltage=345.6V MeasuredBatteryCurrent=-123.4A MeasuredBatteryTemperature=-12.3degC");
}
