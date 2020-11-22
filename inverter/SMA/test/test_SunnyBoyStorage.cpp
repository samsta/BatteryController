#include <gmock/gmock.h>
#include "can/messages/SMA/BatteryIdentity.hpp"
#include "can/messages/SMA/BatteryLimits.hpp"
#include "can/messages/SMA/BatteryManufacturer.hpp"
#include "can/messages/SMA/BatteryMeasurements.hpp"
#include "can/messages/SMA/BatteryName.hpp"
#include "can/messages/SMA/BatteryState.hpp"
#include "can/messages/SMA/BatterySystemInfo.hpp"
#include "can/messages/SMA/Ids.hpp"
#include "can/messages/SMA/InverterCommand.hpp"
#include "can/messages/SMA/InverterIdentity.hpp"
#include "can/messages/SMA/InverterManufacturer.hpp"
#include "core/Callback.hpp"
#include "inverter/SMA/SunnyBoyStorage.hpp"
#include "mocks/can/FrameSink.hpp"
#include "mocks/contactor/Contactor.hpp"
#include "mocks/core/Timer.hpp"
#include "mocks/monitor/Monitor.hpp"

using namespace testing;
using namespace can::messages::SMA;
using namespace can;

namespace inverter {
namespace SMA {
namespace {

MATCHER_P(MatchesMessage, m, "")
{
   return arg == m;
}

#define EXPECT_NO_CALL(o, m) EXPECT_CALL(o, m).Times(0)

class Nonsense: public can::messages::Message
{
public:
   Nonsense(): Message(0x00){}
   virtual void toStream(logging::ostream& os) const
   {
      os << "Nonsense";
   }    
};

TEST(SunnyBoyStorage, registersAndDeregistersTimerCallbackFor5000MillisecondPeriod)
{
   mocks::can::FrameSink sink;
   mocks::core::Timer timer;
   mocks::monitor::Monitor monitor;
   mocks::contactor::Contactor contactor;
   
   {
      core::Invokable* invokable;
      EXPECT_CALL(timer, registerPeriodicCallback(_, 5000)).WillOnce(SaveArg<0>(&invokable));
      SunnyBoyStorage sbs(sink, timer, monitor, contactor);
      EXPECT_CALL(timer, deregisterCallback(invokable));
   }
}

class SunnyBoyStorageAtStartupTest: public Test
{
public:
   SunnyBoyStorageAtStartupTest():
      constructor_expectation(timer, &broadcast_callback),
      sbs(sink, timer, monitor, contactor)
   {
   }

   NiceMock<mocks::can::FrameSink>       sink;
   NiceMock<mocks::core::Timer>          timer;
   NiceMock<mocks::monitor::Monitor>     monitor;
   NiceMock<mocks::contactor::Contactor> contactor;
   core::Invokable*                      broadcast_callback;

   class ConstructorExpectation
   {
   public:
      ConstructorExpectation(mocks::core::Timer& timer, core::Invokable** invokable)
      {
         EXPECT_CALL(timer, registerPeriodicCallback(_, _)).WillOnce(SaveArg<0>(invokable));
      }
   } constructor_expectation;
   
   SunnyBoyStorage sbs;
};

TEST_F(SunnyBoyStorageAtStartupTest, doesntBroadcastIfNothingReceivedFromInverter)
{
   EXPECT_NO_CALL(sink, sink(_));

   broadcast_callback->invoke();
}

TEST_F(SunnyBoyStorageAtStartupTest, broadcastsAfterReceivingInverterManufacturer)
{
   sbs.sink(InverterManufacturer(0, "SMA"));

   EXPECT_CALL(sink, sink(_)).Times(AtLeast(1));
   broadcast_callback->invoke();
}

TEST_F(SunnyBoyStorageAtStartupTest, broadcastsAfterReceivingInverterCommand)
{
   sbs.sink(InverterCommand());

   EXPECT_CALL(sink, sink(_)).Times(AtLeast(1));
   broadcast_callback->invoke();
}

TEST_F(SunnyBoyStorageAtStartupTest, broadcastsAfterReceivingInverterIdentity)
{
   sbs.sink(InverterIdentity());

   EXPECT_CALL(sink, sink(_)).Times(AtLeast(1));
   broadcast_callback->invoke();
}

TEST_F(SunnyBoyStorageAtStartupTest, doesntBroadcastAfterReceivingUnrecognisedMessage)
{
   sbs.sink(Nonsense());

   EXPECT_NO_CALL(sink, sink(_));
   broadcast_callback->invoke();
}

TEST_F(SunnyBoyStorageAtStartupTest, broadcastsStopsAfterNotReceivingMessageFor4Intervals)
{
   sbs.sink(InverterManufacturer(0, "SMA"));

   // 4 intervals means 4 x 5s = 20s
   for (unsigned k = 0; k < 4; k++)
   {
      EXPECT_CALL(sink, sink(_)).Times(AtLeast(1));
      broadcast_callback->invoke();
   }

   EXPECT_NO_CALL(sink, sink(_));
   broadcast_callback->invoke();
}

TEST_F(SunnyBoyStorageAtStartupTest, broadcastsResumeAfterReceivingMessage)
{
   sbs.sink(InverterManufacturer(0, "SMA"));

   for (unsigned k = 0; k < 10; k++)
   {
      broadcast_callback->invoke();
   }

   sbs.sink(InverterManufacturer(0, "SMA"));

   EXPECT_CALL(sink, sink(_)).Times(AtLeast(1));
   broadcast_callback->invoke();
}


class SunnyBoyStorageTest: public SunnyBoyStorageAtStartupTest
{
public:
   SunnyBoyStorageTest(): SunnyBoyStorageAtStartupTest()
   {
      sbs.sink(InverterManufacturer(0, "SMA"));
   }
};

TEST_F(SunnyBoyStorageTest, publishesBatteryStateUponBroadcast)
{
   EXPECT_CALL(monitor, getSocPercent()).WillOnce(Return(87.6));
   EXPECT_CALL(monitor, getSohPercent()).WillOnce(Return(67.8));
   EXPECT_CALL(monitor, getEnergyRemainingKwh()).WillOnce(Return(123.4));
   EXPECT_CALL(monitor, getCapacityKwh()).WillOnce(Return(567.8));

   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
   EXPECT_CALL(sink, sink(MatchesMessage(BatteryState(87.6, 67.8, 123.4, 567.8))));
   
   broadcast_callback->invoke();
}

TEST_F(SunnyBoyStorageTest, publishesBatteryMeasurementsUponBroadcast)
{
   EXPECT_CALL(monitor, getVoltage()).WillOnce(Return(469.9));
   EXPECT_CALL(monitor, getCurrent()).WillOnce(Return(1.6));
   EXPECT_CALL(monitor, getTemperature()).WillOnce(Return(14.1));

   EXPECT_CALL(contactor, isClosed()).WillOnce(Return(true));
   
   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
   EXPECT_CALL(sink, sink(MatchesMessage(BatteryMeasurements()
                                         .setVoltage(469.9)
                                         .setCurrent(1.6)
                                         .setTemperature(14.1)
                                         .setState(BatteryMeasurements::CONNECTED)
                                         .setInverterControlFlags(0))));

   broadcast_callback->invoke();
}

TEST_F(SunnyBoyStorageTest, publishesBatteryLimitsUponBroadcast)
{
   EXPECT_CALL(monitor, getMaxChargeVoltage()).WillOnce(Return(432.1));
   EXPECT_CALL(monitor, getMinDischargeVoltage()).WillOnce(Return(345.6));
   EXPECT_CALL(monitor, getChargeCurrentLimit()).WillOnce(Return(21.3));
   EXPECT_CALL(monitor, getDischargeCurrentLimit()).WillOnce(Return(32.1));

   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
   EXPECT_CALL(sink, sink(MatchesMessage(BatteryLimits()
                                         .setChargeVoltage(432.1)
                                         .setDischargeVoltage(345.6)
                                         .setChargeCurrent(21.3)
                                         .setDischargeCurrent(32.1))));
   
   broadcast_callback->invoke();
}

TEST_F(SunnyBoyStorageTest, publishesBatteryMeasurementsUponBroadcastWithContactorOpen)
{
   ON_CALL(contactor, isClosed()).WillByDefault(Return(false));
   
   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
   EXPECT_CALL(sink, sink(MatchesMessage(BatteryMeasurements()
                                         .setState(BatteryMeasurements::DISCONNECTED))));

   broadcast_callback->invoke();
}

TEST_F(SunnyBoyStorageTest, requestsToCloseContactor)
{
   EXPECT_CALL(contactor, close());

   sbs.sink(InverterCommand().setCommand(InverterCommand::CONNECT));
}

TEST_F(SunnyBoyStorageTest, requestsToOpenContactor)
{
   EXPECT_CALL(contactor, open());

   sbs.sink(InverterCommand().setCommand(InverterCommand::DISCONNECT));
}

TEST_F(SunnyBoyStorageTest, requestsToOpenContactorOnGarbageCommand)
{
   EXPECT_CALL(contactor, open());

   sbs.sink(InverterCommand().setCommand(InverterCommand::Command(77)));
}

TEST_F(SunnyBoyStorageTest, requestsToOpenContactorWhenInverterGoesSilent)
{
   sbs.sink(InverterCommand().setCommand(InverterCommand::CONNECT));

   EXPECT_NO_CALL(contactor, open());

   broadcast_callback->invoke();
   broadcast_callback->invoke();
   broadcast_callback->invoke();
   broadcast_callback->invoke();

   EXPECT_CALL(contactor, open());
   broadcast_callback->invoke();
}


TEST_F(SunnyBoyStorageTest, sendsBatterySystemInfoOnInverterIdentityReceived)
{
   EXPECT_CALL(monitor, getSystemVersion()).WillOnce(Return(123456));
   EXPECT_CALL(monitor, getNominalCapacityKwh()).WillOnce(Return(42));
   EXPECT_CALL(monitor, getNumberOfModules()).WillOnce(Return(7));
   
   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
   EXPECT_CALL(sink, sink(MatchesMessage(BatterySystemInfo(123456, 42, 7, 2))));
   
   sbs.sink(InverterIdentity());
}

TEST_F(SunnyBoyStorageTest, sendsIdentityOnInverterIdentityReceived)
{
   EXPECT_CALL(monitor, getSerialNumber()).WillOnce(Return(123456));
   EXPECT_CALL(monitor, getManufacturingDateUnixTime()).WillOnce(Return(987654));
   
   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
   EXPECT_CALL(sink, sink(MatchesMessage(BatteryIdentity(123456, 987654))));
   
   sbs.sink(InverterIdentity());
}

TEST_F(SunnyBoyStorageTest, sendsManufacturerNameOnInverterIdentityReceived)
{
   EXPECT_CALL(monitor, getManufacturerName()).WillOnce(Return("abcdefghijklmnop"));
   
   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
   EXPECT_CALL(sink, sink(MatchesMessage(BatteryManufacturer(0, "abcdefg"))));
   EXPECT_CALL(sink, sink(MatchesMessage(BatteryManufacturer(1, "hijklmn"))));
   EXPECT_CALL(sink, sink(MatchesMessage(BatteryManufacturer(2, "op"))));
   
   sbs.sink(InverterIdentity());
}

TEST_F(SunnyBoyStorageTest, sendsShortManufacturerNameOnInverterIdentityReceived)
{
   EXPECT_CALL(monitor, getManufacturerName()).WillOnce(Return("ab"));
   
   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
   EXPECT_CALL(sink, sink(MatchesMessage(BatteryManufacturer(0, "ab"))));
   
   sbs.sink(InverterIdentity());
}

TEST_F(SunnyBoyStorageTest, sendBatteryNameOnInverterIdentityReceived)
{
   EXPECT_CALL(monitor, getBatteryName()).WillOnce(Return("qwertyuiop"));
   
   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
   EXPECT_CALL(sink, sink(MatchesMessage(BatteryName(0, "qwertyu"))));
   EXPECT_CALL(sink, sink(MatchesMessage(BatteryName(1, "iop"))));
   
   sbs.sink(InverterIdentity());
}


}
}
}
