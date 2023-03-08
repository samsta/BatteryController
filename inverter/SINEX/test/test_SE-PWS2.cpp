#include <gmock/gmock.h>
#include "can/messages/SINEX/BatteryLimitsOne.hpp"
#include "can/messages/SINEX/BatteryLimitsTwo.hpp"
#include "can/messages/SINEX/InverterHeartbeat.hpp"
#include "core/Callback.hpp"
#include "inverter/SINEX/SE-PWS2.hpp"
#include "mocks/can/FrameSink.hpp"
#include "mocks/contactor/Contactor.hpp"
#include "mocks/core/Timer.hpp"
#include "mocks/monitor/Monitor.hpp"

using namespace testing;
using namespace can::messages::SINEX;
using namespace can;

namespace inverter {
namespace SINEX {
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

TEST(SE_PWS2, registersAndDeregistersTimerCallbackFor500MillisecondPeriod)
{
  mocks::can::FrameSink sink;
  mocks::core::Timer timer;
  mocks::monitor::Monitor monitor;
  mocks::contactor::Contactor contactor;

  {
     core::Invokable* invokable;
     EXPECT_CALL(timer, registerPeriodicCallback(_, 500)).WillOnce(SaveArg<0>(&invokable));
     SE_PWS2 sbs(sink, timer, monitor, contactor, nullptr);
     EXPECT_CALL(timer, deregisterCallback(invokable));
  }
}

class SE_PWS2AtStartupTest: public Test
{
public:
  SE_PWS2AtStartupTest():
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

  SE_PWS2 sbs;
};

TEST_F(SE_PWS2AtStartupTest, getHbCount1)
{
  sbs.sink(InverterHeartbeat(can::StandardDataFrame("98160127#0001000000000000")));

  EXPECT_EQ(sbs.getHeartbeatValue(), 1);
}

TEST_F(SE_PWS2AtStartupTest, getHbCountFFFF)
{
  sbs.sink(InverterHeartbeat(can::StandardDataFrame("98160127#FFFF000000000000")));

  EXPECT_EQ(sbs.getHeartbeatValue(), 0xFFFF);
}

TEST_F(SE_PWS2AtStartupTest, testHbNonConsec)
{
  sbs.sink(InverterHeartbeat(can::StandardDataFrame("98160127#0001000000000000")));
  sbs.sink(InverterHeartbeat(can::StandardDataFrame("98160127#0002000000000000")));
  EXPECT_FALSE(sbs.getHbNonConsec());
  sbs.sink(InverterHeartbeat(can::StandardDataFrame("98160127#0004000000000000")));
  EXPECT_TRUE(sbs.getHbNonConsec());
}

TEST_F(SE_PWS2AtStartupTest, testHbConsecNearRollover)
{
  sbs.sink(InverterHeartbeat(can::StandardDataFrame("98160127#FFFE000000000000")));
  sbs.sink(InverterHeartbeat(can::StandardDataFrame("98160127#FFFF000000000000")));
  EXPECT_FALSE(sbs.getHbNonConsec());
}

TEST_F(SE_PWS2AtStartupTest, testHbConsecAtRollover)
{
  sbs.sink(InverterHeartbeat(can::StandardDataFrame("98160127#FFFF000000000000")));
  sbs.sink(InverterHeartbeat(can::StandardDataFrame("98160127#0000000000000000")));
  EXPECT_FALSE(sbs.getHbNonConsec());
}

//TEST_F(SE_PWS2AtStartupTest, doesntBroadcastIfNothingReceivedFromInverter)
//{
//   EXPECT_NO_CALL(sink, sink(_));
//}
//
//TEST_F(SE_PWS2AtStartupTest, doesntbroadcastsAfterReceivingInverterInfoRequestAndContactsAreOpen)
//{
//   EXPECT_NO_CALL(sink, sink(_));
//   ON_CALL(contactor, isClosed()).WillByDefault(Return(false));
//
//   sbs.sink(InverterInfoRequest());
//}
//
//TEST_F(SE_PWS2AtStartupTest, broadcastsAfterReceivingInverterInfoRequestAndContactsClosed)
//{
//   EXPECT_CALL(sink, sink(_)).Times(AtLeast(1));
//   ON_CALL(contactor, isClosed()).WillByDefault(Return(true));
//
//   sbs.sink(InverterInfoRequest());
//}
//
// TEST_F(SE_PWS2AtStartupTest, requestsToCloseContactor)
// {
//   EXPECT_CALL(contactor, close());

//   sbs.sink(InverterInfoRequest(can::StandardDataFrame("4200#0000000000000000")));
// }
//
//TEST_F(SE_PWS2AtStartupTest, requestsToOpenContactorWhenInverterNotAlive)
//{
//   EXPECT_NO_CALL(contactor, open());
//
//   inverter_alive_callback->invoke();
//   inverter_alive_callback->invoke();
//   inverter_alive_callback->invoke();
//   inverter_alive_callback->invoke();
//   inverter_alive_callback->invoke();
//
//   EXPECT_CALL(contactor, open());
//   inverter_alive_callback->invoke();
//}
//
//TEST_F(SE_PWS2AtStartupTest, requestsToCloseContactorWhenInverterAliveAgain)
//{
//   EXPECT_NO_CALL(contactor, open());
//
//   inverter_alive_callback->invoke();
//   inverter_alive_callback->invoke();
//   inverter_alive_callback->invoke();
//   inverter_alive_callback->invoke();
//   inverter_alive_callback->invoke();
//
//   EXPECT_CALL(contactor, open());
//   inverter_alive_callback->invoke();
//
//   EXPECT_CALL(contactor, close());
//   sbs.sink(InverterInfoRequest(can::StandardDataFrame("4200#0000000000000000")));
//}
//
//TEST_F(SE_PWS2AtStartupTest, publishesBatteryInfoUponRequest)
//{
//   EXPECT_CALL(monitor, getVoltage()).WillOnce(Return(395.3));
//   EXPECT_CALL(monitor, getCurrent()).WillOnce(Return(5.55));
//   EXPECT_CALL(monitor, getTemperature()).WillOnce(Return(16.7));
//   EXPECT_CALL(monitor, getSocPercent()).WillOnce(Return(87));
//   EXPECT_CALL(monitor, getSohPercent()).WillOnce(Return(67));
//
//   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryInfo()
//                                       .setPileVoltage(395.3)
//                                       .setPileCurrent(5.55)
//                                       .setBMS2ndTemp(16.7)
//                                       .setSOC(87)
//                                       .setSOH(67))));
//   ON_CALL(contactor, isClosed()).WillByDefault(Return(true));
//
//   sbs.sink(InverterInfoRequest(can::StandardDataFrame("4200#0000000000000000")));
//}
//
//TEST_F(SE_PWS2AtStartupTest, publishesBatteryLimitsUponRequest)
//{
//   EXPECT_CALL(monitor, getMaxChargeVoltage()).WillOnce(Return(432.1));
//   EXPECT_CALL(monitor, getMinDischargeVoltage()).WillOnce(Return(345.6));
//   EXPECT_CALL(monitor, getChargeCurrentLimit()).WillOnce(Return(10.9));
//   EXPECT_CALL(monitor, getDischargeCurrentLimit()).WillOnce(Return(11.1));
//
//   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryLimits()
//                                         .setChargeVoltage(432.1)
//                                         .setDischargeVoltage(345.6)
//                                         .setChargeCurrent(10.9)
//                                         .setDischargeCurrent(11.1))));
//   ON_CALL(contactor, isClosed()).WillByDefault(Return(true));
//
//   sbs.sink(InverterInfoRequest(can::StandardDataFrame("4200#0000000000000000")));
//}
//
//TEST_F(SE_PWS2AtStartupTest, publishesBatteryCellVoltInfoUponRequest)
//{
//   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryCellVoltInfo()
//                                          .setMaxSingleCellVoltage(3.980)
//                                          .setMinSingleCellVoltage(3.978)
//                                          .setMaxCellVoltageNumber(1)
//                                          .setMinCellVoltageNumber(2))));
//   ON_CALL(contactor, isClosed()).WillByDefault(Return(true));
//
//   sbs.sink(InverterInfoRequest(can::StandardDataFrame("4200#0000000000000000")));
//}
//
//TEST_F(SE_PWS2AtStartupTest, publishesBatteryCellTempInfoUponRequest)
//{
//   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryCellTempInfo()
//                                          .setMaxSingleCellTemp(16)
//                                          .setMinSingleCellTemp(14)
//                                          .setMaxCellTempNumber(3)
//                                          .setMinCellTempNumber(4))));
//   ON_CALL(contactor, isClosed()).WillByDefault(Return(true));
//
//   sbs.sink(InverterInfoRequest(can::StandardDataFrame("4200#0000000000000000")));
//}
//
//TEST_F(SE_PWS2AtStartupTest, publishesBatteryStatusUponRequest)
//{
//   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryStatus(BatteryStatus::BASIC_STATUS_IDLE))));
//   ON_CALL(contactor, isClosed()).WillByDefault(Return(true));
//
//   sbs.sink(InverterInfoRequest(can::StandardDataFrame("4200#0000000000000000")));
//}
//
//TEST_F(SE_PWS2AtStartupTest, publishesBatteryModVoltInfoUponRequest)
//{
//   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryModVoltInfo()
//                                          .setMaxSingleModuleVoltage(2 * 3.980)
//                                          .setMinSingleModuleVoltage(2 * 3.978)
//                                          .setMaxModuleVoltageNumber(1)
//                                          .setMinModuleVoltageNumber(2))));
//   ON_CALL(contactor, isClosed()).WillByDefault(Return(true));
//
//   sbs.sink(InverterInfoRequest(can::StandardDataFrame("4200#0000000000000000")));
//}
//
//TEST_F(SE_PWS2AtStartupTest, publishesBatteryModTempInfoUponRequest)
//{
//   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryModTempInfo()
//                                          .setMaxSingleModuleTemp(16)
//                                          .setMinSingleModuleTemp(14)
//                                          .setMaxModuleTempNumber(3)
//                                          .setMinModuleTempNumber(4))));
//   ON_CALL(contactor, isClosed()).WillByDefault(Return(true));
//
//   sbs.sink(InverterInfoRequest(can::StandardDataFrame("4200#0000000000000000")));
//}
//
//TEST_F(SE_PWS2AtStartupTest, publishesBatteryForbiddenUponRequest)
//{
//   EXPECT_CALL(sink, sink(_)).Times(AnyNumber());
//   EXPECT_CALL(sink, sink(MatchesMessage(BatteryForbidden())));
//   ON_CALL(contactor, isClosed()).WillByDefault(Return(true));
//
//   sbs.sink(InverterInfoRequest(can::StandardDataFrame("4200#0000000000000000")));
//}
//
}
}
}
