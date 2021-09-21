#include <gmock/gmock.h>
#include "can/messages/TSUN/BatteryCellTempInfo.hpp"
#include "can/messages/TSUN/BatteryCellVoltInfo.hpp"
#include "can/messages/TSUN/BatteryForbidden.hpp"
#include "can/messages/TSUN/BatteryInfo.hpp"
#include "can/messages/TSUN/BatteryLimits.hpp"
#include "can/messages/TSUN/BatteryModTempInfo.hpp"
#include "can/messages/TSUN/BatteryModVoltInfo.hpp"
#include "can/messages/TSUN/BatteryStatus.hpp"
#include "can/messages/TSUN/InverterChargeDischargeCmd.hpp"
#include "can/messages/TSUN/InverterInfoRequest.hpp"
#include "can/messages/TSUN/InverterSleepAwakeCmd.hpp"
#include "core/Callback.hpp"
#include "inverter/TSUN/TSOL-H50K.hpp"
#include "mocks/can/FrameSink.hpp"
#include "mocks/contactor/Contactor.hpp"
#include "mocks/core/Timer.hpp"
#include "mocks/monitor/Monitor.hpp"

using namespace testing;
using namespace can::messages::TSUN;
using namespace can;

namespace inverter {
namespace TSUN {
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

TEST(TSOL_H50K, registersAndDeregistersTimerCallbackFor5000MillisecondPeriod)
{
   mocks::can::FrameSink sink;
   mocks::core::Timer timer;
   mocks::monitor::Monitor monitor;
   mocks::contactor::Contactor contactor;

   {
      core::Invokable* invokable;
      EXPECT_CALL(timer, registerPeriodicCallback(_, 5000)).WillOnce(SaveArg<0>(&invokable));
      TSOL_H50K sbs(sink, timer, monitor, contactor);
      EXPECT_CALL(timer, deregisterCallback(invokable));
   }
}

class TSOL_H50KAtStartupTest: public Test
{
public:
   TSOL_H50KAtStartupTest():
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

   TSOL_H50K sbs;
};

TEST_F(TSOL_H50KAtStartupTest, doesntBroadcastIfNothingReceivedFromInverter)
{
   EXPECT_NO_CALL(sink, sink(_));

   broadcast_callback->invoke();
}

TEST_F(TSOL_H50KAtStartupTest, broadcastsAfterReceivingInverterInfoRequest)
{
   sbs.sink(InverterInfoRequest());

   EXPECT_CALL(sink, sink(_)).Times(AtLeast(1));
   broadcast_callback->invoke();
}

//TEST_F(TSOL_H50KAtStartupTest, broadcastsAfterReceivingInverterIdentity)
//{
//   sbs.sink(InverterIdentity());
//
//   EXPECT_CALL(sink, sink(_)).Times(AtLeast(1));
//   broadcast_callback->invoke();
//}
//
//TEST_F(TSOL_H50KAtStartupTest, doesntBroadcastAfterReceivingUnrecognisedMessage)
//{
//   sbs.sink(Nonsense());
//
//   EXPECT_NO_CALL(sink, sink(_));
//   broadcast_callback->invoke();
//}
//
//TEST_F(TSOL_H50KAtStartupTest, broadcastsStopsAfterNotReceivingMessageFor4Intervals)
//{
//   sbs.sink(InverterManufacturer(0, "TSUN"));
//
//   // 4 intervals means 4 x 5s = 20s
//   for (unsigned k = 0; k < 4; k++)
//   {
//      EXPECT_CALL(sink, sink(_)).Times(AtLeast(1));
//      broadcast_callback->invoke();
//   }
//
//   EXPECT_NO_CALL(sink, sink(_));
//   broadcast_callback->invoke();
//}
//
//TEST_F(TSOL_H50KAtStartupTest, broadcastsResumeAfterReceivingMessage)
//{
//   sbs.sink(InverterManufacturer(0, "TSUN"));
//
//   for (unsigned k = 0; k < 10; k++)
//   {
//      broadcast_callback->invoke();
//   }
//
//   sbs.sink(InverterManufacturer(0, "TSUN"));
//
//   EXPECT_CALL(sink, sink(_)).Times(AtLeast(1));
//   broadcast_callback->invoke();
//}
//
//
//class TSOL_H50KTest: public TSOL_H50KAtStartupTest
//{
//public:
//   TSOL_H50KTest(): TSOL_H50KAtStartupTest()
//   {
//      sbs.sink(InverterManufacturer(0, "TSUN"));
//   }
//};
//
//TEST_F(TSOL_H50KTest, publishesBatteryStateUponBroadcast)
//{
//   EXPECT_CALL(monitor, getSocPercent()).WillOnce(Return(87.6));
//   EXPECT_CALL(monitor, getSohPercent()).WillOnce(Return(67.8));
//   EXPECT_CALL(monitor, getEnergyRemainingKwh()).WillOnce(Return(123.4));
//   EXPECT_CALL(monitor, getCapacityKwh()).WillOnce(Return(567.8));
//
//   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryState(87.6, 67.8, 123.4, 567.8))));
//
//   broadcast_callback->invoke();
//}
//
//TEST_F(TSOL_H50KTest, publishesBatteryMeasurementsUponBroadcast)
//{
//   EXPECT_CALL(monitor, getVoltage()).WillOnce(Return(469.9));
//   EXPECT_CALL(monitor, getCurrent()).WillOnce(Return(1.6));
//   EXPECT_CALL(monitor, getTemperature()).WillOnce(Return(14.1));
//
//   EXPECT_CALL(contactor, isClosed()).WillOnce(Return(true));
//
//   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryMeasurements()
//                                         .setVoltage(469.9)
//                                         .setCurrent(1.6)
//                                         .setTemperature(14.1)
//                                         .setState(BatteryMeasurements::CONNECTED)
//                                         .setInverterControlFlags(0))));
//
//   broadcast_callback->invoke();
//}
//
//TEST_F(TSOL_H50KTest, publishesBatteryLimitsUponBroadcast)
//{
//   EXPECT_CALL(monitor, getMaxChargeVoltage()).WillOnce(Return(432.1));
//   EXPECT_CALL(monitor, getMinDischargeVoltage()).WillOnce(Return(345.6));
//   EXPECT_CALL(monitor, getChargeCurrentLimit()).WillOnce(Return(21.3));
//   EXPECT_CALL(monitor, getDischargeCurrentLimit()).WillOnce(Return(32.1));
//
//   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryLimits()
//                                         .setChargeVoltage(432.1)
//                                         .setDischargeVoltage(345.6)
//                                         .setChargeCurrent(21.3)
//                                         .setDischargeCurrent(32.1))));
//
//   broadcast_callback->invoke();
//}
//
//TEST_F(TSOL_H50KTest, publishesBatteryMeasurementsUponBroadcastWithContactorOpen)
//{
//   ON_CALL(contactor, isClosed()).WillByDefault(Return(false));
//
//   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryMeasurements()
//                                         .setState(BatteryMeasurements::DISCONNECTED))));
//
//   broadcast_callback->invoke();
//}
//
//TEST_F(TSOL_H50KTest, requestsToCloseContactor)
//{
//   EXPECT_CALL(contactor, close());
//
//   sbs.sink(InverterCommand().setCommand(InverterCommand::CONNECT));
//}
//
//TEST_F(TSOL_H50KTest, requestsToOpenContactor)
//{
//   EXPECT_CALL(contactor, open());
//
//   sbs.sink(InverterCommand().setCommand(InverterCommand::DISCONNECT));
//}
//
//TEST_F(TSOL_H50KTest, requestsToOpenContactorOnGarbageCommand)
//{
//   EXPECT_CALL(contactor, open());
//
//   sbs.sink(InverterCommand().setCommand(InverterCommand::Command(77)));
//}
//
//TEST_F(TSOL_H50KTest, requestsToOpenContactorWhenInverterGoesSilent)
//{
//   sbs.sink(InverterCommand().setCommand(InverterCommand::CONNECT));
//
//   EXPECT_NO_CALL(contactor, open());
//
//   broadcast_callback->invoke();
//   broadcast_callback->invoke();
//   broadcast_callback->invoke();
//   broadcast_callback->invoke();
//
//   EXPECT_CALL(contactor, open());
//   broadcast_callback->invoke();
//}
//
//
//TEST_F(TSOL_H50KTest, sendsBatterySystemInfoOnInverterIdentityReceived)
//{
//   EXPECT_CALL(monitor, getSystemVersion()).WillOnce(Return(123456));
//   EXPECT_CALL(monitor, getNominalCapacityKwh()).WillOnce(Return(42));
//   EXPECT_CALL(monitor, getNumberOfModules()).WillOnce(Return(7));
//
//   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
//   EXPECT_CALL(sink, sink(MatchesMessage(BatterySystemInfo(123456, 42, 7, 2))));
//
//   sbs.sink(InverterIdentity());
//}
//
//TEST_F(TSOL_H50KTest, sendsIdentityOnInverterIdentityReceived)
//{
//   EXPECT_CALL(monitor, getSerialNumber()).WillOnce(Return(123456));
//   EXPECT_CALL(monitor, getManufacturingDateUnixTime()).WillOnce(Return(987654));
//
//   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryIdentity(123456, 987654))));
//
//   sbs.sink(InverterIdentity());
//}
//
//TEST_F(TSOL_H50KTest, sendsManufacturerNameOnInverterIdentityReceived)
//{
//   EXPECT_CALL(monitor, getManufacturerName()).WillOnce(Return("abcdefghijklmnop"));
//
//   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryManufacturer(0, "abcdefg"))));
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryManufacturer(1, "hijklmn"))));
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryManufacturer(2, "op"))));
//   // should always be followed by a string with just zeros
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryManufacturer(3, ""))));
//
//   sbs.sink(InverterIdentity());
//}
//
//TEST_F(TSOL_H50KTest, sendsShortManufacturerNameOnInverterIdentityReceived)
//{
//   EXPECT_CALL(monitor, getManufacturerName()).WillOnce(Return("ab"));
//
//   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryManufacturer(0, "ab"))));
//   // should always be followed by a string with just zeros
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryManufacturer(1, ""))));
//
//   sbs.sink(InverterIdentity());
//}
//
//TEST_F(TSOL_H50KTest, sendBatteryNameOnInverterIdentityReceived)
//{
//   EXPECT_CALL(monitor, getBatteryName()).WillOnce(Return("qwertyuiop"));
//
//   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryName(0, "qwertyu"))));
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryName(1, "iop"))));
//   // should always be followed by a string with just zeros
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryName(2, ""))));
//
//   sbs.sink(InverterIdentity());
//}


}
}
}
