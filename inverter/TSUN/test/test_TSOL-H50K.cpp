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
      TSOL_H50K sbs(sink, timer, monitor, contactor, nullptr);
      EXPECT_CALL(timer, deregisterCallback(invokable));
   }
}

class TSOL_H50KAtStartupTest: public Test
{
public:
   TSOL_H50KAtStartupTest():
      constructor_expectation(timer, &inverter_alive_callback),
      sbs(sink, timer, monitor, contactor, nullptr)
   {
   }

   NiceMock<mocks::can::FrameSink>       sink;
   NiceMock<mocks::core::Timer>          timer;
   NiceMock<mocks::monitor::Monitor>     monitor;
   NiceMock<mocks::contactor::Contactor> contactor;
   core::Invokable*                      inverter_alive_callback;

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
}

TEST_F(TSOL_H50KAtStartupTest, doesntbroadcastsAfterReceivingInverterInfoRequestAndContactsAreOpen)
{
   EXPECT_NO_CALL(sink, sink(_));
   ON_CALL(contactor, isClosed()).WillByDefault(Return(false));

   sbs.sink(InverterInfoRequest());
}

TEST_F(TSOL_H50KAtStartupTest, broadcastsAfterReceivingInverterInfoRequestAndContactsClosed)
{
   EXPECT_CALL(sink, sink(_)).Times(AtLeast(1));
   ON_CALL(contactor, isClosed()).WillByDefault(Return(true));

   sbs.sink(InverterInfoRequest());
}

TEST_F(TSOL_H50KAtStartupTest, requestsToCloseContactor)
{
   EXPECT_CALL(contactor, close());

   sbs.sink(InverterInfoRequest(can::StandardDataFrame("4200#0000000000000000")));
}

TEST_F(TSOL_H50KAtStartupTest, requestsToOpenContactorWhenInverterNotAlive)
{
   EXPECT_NO_CALL(contactor, open());

   inverter_alive_callback->invoke();
   inverter_alive_callback->invoke();
   inverter_alive_callback->invoke();
   inverter_alive_callback->invoke();
   inverter_alive_callback->invoke();

   EXPECT_CALL(contactor, open());
   inverter_alive_callback->invoke();
}

TEST_F(TSOL_H50KAtStartupTest, requestsToCloseContactorWhenInverterAliveAgain)
{
   EXPECT_NO_CALL(contactor, open());

   inverter_alive_callback->invoke();
   inverter_alive_callback->invoke();
   inverter_alive_callback->invoke();
   inverter_alive_callback->invoke();
   inverter_alive_callback->invoke();

   EXPECT_CALL(contactor, open());
   inverter_alive_callback->invoke();

   EXPECT_CALL(contactor, close());
   sbs.sink(InverterInfoRequest(can::StandardDataFrame("4200#0000000000000000")));
}

TEST_F(TSOL_H50KAtStartupTest, publishesBatteryInfoUponRequest)
{
   EXPECT_CALL(monitor, getVoltage()).WillOnce(Return(395.3));
   EXPECT_CALL(monitor, getCurrent()).WillOnce(Return(5.55));
   EXPECT_CALL(monitor, getTemperature()).WillOnce(Return(16.7));
   EXPECT_CALL(monitor, getSocPercent()).WillOnce(Return(87));
   EXPECT_CALL(monitor, getSohPercent()).WillOnce(Return(67));

   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
   EXPECT_CALL(sink, sink(MatchesMessage(BatteryInfo()
                                       .setPileVoltage(395.3)
                                       .setPileCurrent(5.55)
                                       .setBMS2ndTemp(16.7)
                                       .setSOC(87)
                                       .setSOH(67))));
   ON_CALL(contactor, isClosed()).WillByDefault(Return(true));

   sbs.sink(InverterInfoRequest(can::StandardDataFrame("4200#0000000000000000")));
}

TEST_F(TSOL_H50KAtStartupTest, publishesBatteryLimitsUponRequest)
{
   EXPECT_CALL(monitor, getMaxChargeVoltage()).WillOnce(Return(432.1));
   EXPECT_CALL(monitor, getMinDischargeVoltage()).WillOnce(Return(345.6));
   EXPECT_CALL(monitor, getChargeCurrentLimit()).WillOnce(Return(10.9));
   EXPECT_CALL(monitor, getDischargeCurrentLimit()).WillOnce(Return(11.1));

   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
   EXPECT_CALL(sink, sink(MatchesMessage(BatteryLimits()
                                         .setChargeVoltage(432.1)
                                         .setDischargeVoltage(345.6)
                                         .setChargeCurrent(10.9)
                                         .setDischargeCurrent(11.1))));
   ON_CALL(contactor, isClosed()).WillByDefault(Return(true));

   sbs.sink(InverterInfoRequest(can::StandardDataFrame("4200#0000000000000000")));
}

TEST_F(TSOL_H50KAtStartupTest, publishesBatteryCellVoltInfoUponRequest)
{
   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
   EXPECT_CALL(sink, sink(MatchesMessage(BatteryCellVoltInfo()
                                          .setMaxSingleCellVoltage(3.980)
                                          .setMinSingleCellVoltage(3.978)
                                          .setMaxCellVoltageNumber(1)
                                          .setMinCellVoltageNumber(2))));
   ON_CALL(contactor, isClosed()).WillByDefault(Return(true));

   sbs.sink(InverterInfoRequest(can::StandardDataFrame("4200#0000000000000000")));
}

TEST_F(TSOL_H50KAtStartupTest, publishesBatteryCellTempInfoUponRequest)
{
   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
   EXPECT_CALL(sink, sink(MatchesMessage(BatteryCellTempInfo()
                                          .setMaxSingleCellTemp(16)
                                          .setMinSingleCellTemp(14)
                                          .setMaxCellTempNumber(3)
                                          .setMinCellTempNumber(4))));
   ON_CALL(contactor, isClosed()).WillByDefault(Return(true));

   sbs.sink(InverterInfoRequest(can::StandardDataFrame("4200#0000000000000000")));
}

TEST_F(TSOL_H50KAtStartupTest, publishesBatteryStatusUponRequest)
{
   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
   EXPECT_CALL(sink, sink(MatchesMessage(BatteryStatus(BatteryStatus::BASIC_STATUS_IDLE))));
   ON_CALL(contactor, isClosed()).WillByDefault(Return(true));

   sbs.sink(InverterInfoRequest(can::StandardDataFrame("4200#0000000000000000")));
}

TEST_F(TSOL_H50KAtStartupTest, publishesBatteryModVoltInfoUponRequest)
{
   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
   EXPECT_CALL(sink, sink(MatchesMessage(BatteryModVoltInfo()
                                          .setMaxSingleModuleVoltage(2 * 3.980)
                                          .setMinSingleModuleVoltage(2 * 3.978)
                                          .setMaxModuleVoltageNumber(1)
                                          .setMinModuleVoltageNumber(2))));
   ON_CALL(contactor, isClosed()).WillByDefault(Return(true));

   sbs.sink(InverterInfoRequest(can::StandardDataFrame("4200#0000000000000000")));
}

TEST_F(TSOL_H50KAtStartupTest, publishesBatteryModTempInfoUponRequest)
{
   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
   EXPECT_CALL(sink, sink(MatchesMessage(BatteryModTempInfo()
                                          .setMaxSingleModuleTemp(16)
                                          .setMinSingleModuleTemp(14)
                                          .setMaxModuleTempNumber(3)
                                          .setMinModuleTempNumber(4))));
   ON_CALL(contactor, isClosed()).WillByDefault(Return(true));

   sbs.sink(InverterInfoRequest(can::StandardDataFrame("4200#0000000000000000")));
}

TEST_F(TSOL_H50KAtStartupTest, publishesBatteryForbiddenUponRequest)
{
   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
   EXPECT_CALL(sink, sink(MatchesMessage(BatteryForbidden())));
   ON_CALL(contactor, isClosed()).WillByDefault(Return(true));

   sbs.sink(InverterInfoRequest(can::StandardDataFrame("4200#0000000000000000")));
}

}
}
}
