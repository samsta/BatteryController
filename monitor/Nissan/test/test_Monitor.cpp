#include <gmock/gmock.h>
#include <limits>
#include "monitor/Nissan/Monitor.hpp"
#include "mocks/contactor/Contactor.hpp"
#include "can/messages/Nissan/PackTemperatures.hpp"
#include "can/messages/Nissan/CellVoltageRange.hpp"
#include "can/messages/Nissan/BatteryState.hpp"
#include "can/messages/Nissan/BatteryStatus.hpp"

using namespace testing;
using namespace monitor::Nissan;
using namespace can::messages::Nissan;
using std::isnan;

namespace {

const float ARBITRARY_SAFE_VOLTAGE = 3.8;
const float CRITICALLY_HIGH_VOLTAGE = 4.15;
const float CRITICALLY_LOW_VOLTAGE = 3.0;
const float CRITICAL_SPREAD_VOLTAGE = 0.1;

const float ARBITRARY_SAFE_TEMPERATURE = 20.0;
const float CRITICALLY_HIGH_TEMPERATURE = 50;
const float CRITICALLY_LOW_TEMPERATURE = 2;
const float WARN_HIGH_TEMPERATURE = 40;
const float WARN_LOW_TEMPERATURE = 5;

const float NOMINAL_CURRENT_LIMIT = 20;

const float TOLERANCE = 0.0001;

CellVoltageRange goodCellVoltageRange()
{
   return CellVoltageRange().setMin(ARBITRARY_SAFE_VOLTAGE).setMax(ARBITRARY_SAFE_VOLTAGE);
}

PackTemperatures allPackTemperatures(float temperature)
{
   PackTemperatures temperatures;
   for (unsigned k = 0; k < PackTemperatures::NUM_SENSORS; k++)
   {
      temperatures.setTemperature(k, temperature);
   }
   return temperatures;
}

PackTemperatures goodPackTemperatures()
{
   return allPackTemperatures(ARBITRARY_SAFE_TEMPERATURE);
}

TEST(Monitor, contactorDeclaredUnsafeUponConstruction)
{
   NiceMock<mocks::contactor::Contactor> contactor;

   EXPECT_CALL(contactor, setSafeToOperate(false));

   Monitor monitor(contactor);
}

class MonitorConstructed: public Test
{
public:
   MonitorConstructed(): monitor(contactor)
   {
   }

   NiceMock<mocks::contactor::Contactor> contactor;
   Monitor monitor;
};

TEST_F(MonitorConstructed, contactorNotDeclaredSafeWithOnlyGoodCellVoltageRange)
{
   EXPECT_CALL(contactor, setSafeToOperate(true)).Times(0);

   monitor.sink(goodCellVoltageRange());
}

TEST_F(MonitorConstructed, contactorNotDeclaredSafeWithOnlyGoodPackTemperatures)
{
   EXPECT_CALL(contactor, setSafeToOperate(true)).Times(0);

   monitor.sink(goodPackTemperatures());
}

TEST_F(MonitorConstructed, contactorDeclaredSafeWithGoodCellVoltageRangeFollowedByGoodPackTemperatures)
{
   EXPECT_CALL(contactor, setSafeToOperate(true));

   monitor.sink(goodCellVoltageRange());
   monitor.sink(goodPackTemperatures());
}

TEST_F(MonitorConstructed, contactorDeclaredSafeWithGoodCellVoltageRangeFollowedByGoodPackTemperaturesWithOneMissingSensor)
{
   EXPECT_CALL(contactor, setSafeToOperate(true));

   monitor.sink(goodCellVoltageRange());
   monitor.sink(goodPackTemperatures()
                .setTemperature(0, NAN));
}

TEST_F(MonitorConstructed, contactorNotDeclaredSafeWithGoodCellVoltageRangeFollowedByGoodPackTemperaturesWithTwoMissingSensors)
{
   EXPECT_CALL(contactor, setSafeToOperate(true)).Times(0);

   monitor.sink(goodCellVoltageRange());
   monitor.sink(goodPackTemperatures()
                .setTemperature(0, NAN)
                .setTemperature(1, NAN));
}

TEST_F(MonitorConstructed, contactorDeclaredSafeWithGoodPackTemperaturesFollowedByGoodCellVoltageRange)
{
   EXPECT_CALL(contactor, setSafeToOperate(true));

   monitor.sink(goodPackTemperatures());
   monitor.sink(goodCellVoltageRange());
}

TEST_F(MonitorConstructed, contactorNotDeclaredSafeWithGoodCellVoltageRangeAndInvalidPackTemperatures)
{
   EXPECT_CALL(contactor, setSafeToOperate(true)).Times(0);

   monitor.sink(goodCellVoltageRange());
   monitor.sink(PackTemperatures());
}

TEST_F(MonitorConstructed, contactorNotDeclaredSafeWithGoodPackTemperaturesAndInvalidCellVoltageRange)
{
   EXPECT_CALL(contactor, setSafeToOperate(true)).Times(0);

   monitor.sink(goodPackTemperatures());
   monitor.sink(CellVoltageRange());
}

class MonitorSafeToOperate: public MonitorConstructed
{
public:
   MonitorSafeToOperate()
   {
      monitor.sink(goodPackTemperatures());
      monitor.sink(goodCellVoltageRange());
   }
};

TEST_F(MonitorSafeToOperate, contactorNoAdditionalDeclarationWithGoodVoltageRange)
{
   EXPECT_CALL(contactor, setSafeToOperate(_)).Times(0);

   monitor.sink(goodCellVoltageRange());
}

TEST_F(MonitorSafeToOperate, contactorDeclaredUnsafeWhenVoltageCriticallyHigh)
{
   EXPECT_CALL(contactor, setSafeToOperate(false));

   monitor.sink(CellVoltageRange()
                .setMin(CRITICALLY_HIGH_VOLTAGE - TOLERANCE)
                .setMax(CRITICALLY_HIGH_VOLTAGE));
}

TEST_F(MonitorSafeToOperate, contactorNotDeclaredUnsafeWhenVoltageJustBelowCriticallyHigh)
{
   EXPECT_CALL(contactor, setSafeToOperate(false)).Times(0);

   monitor.sink(CellVoltageRange()
                .setMin(CRITICALLY_HIGH_VOLTAGE - TOLERANCE)
                .setMax(CRITICALLY_HIGH_VOLTAGE - TOLERANCE));
}

TEST_F(MonitorSafeToOperate, contactorDeclaredUnsafeWhenVoltageCriticallyLow)
{
   EXPECT_CALL(contactor, setSafeToOperate(false));

   monitor.sink(CellVoltageRange()
                .setMin(CRITICALLY_LOW_VOLTAGE)
                .setMax(CRITICALLY_LOW_VOLTAGE + TOLERANCE));
}

TEST_F(MonitorSafeToOperate, contactorNotDeclaredUnsafeWhenVoltageJustAboveCriticallyLow)
{
   EXPECT_CALL(contactor, setSafeToOperate(false)).Times(0);

   monitor.sink(CellVoltageRange()
                .setMin(CRITICALLY_LOW_VOLTAGE + TOLERANCE)
                .setMax(CRITICALLY_LOW_VOLTAGE + TOLERANCE));
}

TEST_F(MonitorSafeToOperate, contactorDeclaredUnsafeWhenVoltageRangeSpreadCritical)
{
   EXPECT_CALL(contactor, setSafeToOperate(false));

   monitor.sink(CellVoltageRange()
                .setMin(ARBITRARY_SAFE_VOLTAGE)
                .setMax(ARBITRARY_SAFE_VOLTAGE + CRITICAL_SPREAD_VOLTAGE + TOLERANCE));
}

TEST_F(MonitorSafeToOperate, contactorNotDeclaredUnsafeWhenVoltageRangeSpreadJustBelowCritical)
{
   EXPECT_CALL(contactor, setSafeToOperate(false)).Times(0);

   monitor.sink(CellVoltageRange()
                .setMin(ARBITRARY_SAFE_VOLTAGE)
                .setMax(ARBITRARY_SAFE_VOLTAGE + CRITICAL_SPREAD_VOLTAGE - TOLERANCE));
}

TEST_F(MonitorSafeToOperate, contactorNoAdditionalDeclarationWithGoodTemperatures)
{
   EXPECT_CALL(contactor, setSafeToOperate(_)).Times(0);

   monitor.sink(goodPackTemperatures());
}

TEST_F(MonitorSafeToOperate, contactorDeclaredUnsafeWhenTemperatureCriticallyHigh)
{
   EXPECT_CALL(contactor, setSafeToOperate(false));

   monitor.sink(allPackTemperatures(CRITICALLY_HIGH_TEMPERATURE));
}

TEST_F(MonitorSafeToOperate, contactorNotDeclaredUnsafeWhenTemperatureJustBelowCriticallyHigh)
{
   EXPECT_CALL(contactor, setSafeToOperate(false)).Times(0);

   monitor.sink(allPackTemperatures(CRITICALLY_HIGH_TEMPERATURE - TOLERANCE));
}

TEST_F(MonitorSafeToOperate, contactorDeclaredUnsafeWhenTemperatureCriticallyLow)
{
   EXPECT_CALL(contactor, setSafeToOperate(false));

   monitor.sink(allPackTemperatures(CRITICALLY_LOW_TEMPERATURE));
}


TEST_F(MonitorSafeToOperate, contactorNotDeclaredUnsafeWhenTemperatureJustAboveCriticallyLow)
{
   EXPECT_CALL(contactor, setSafeToOperate(false)).Times(0);

   monitor.sink(allPackTemperatures(CRITICALLY_LOW_TEMPERATURE + TOLERANCE));
}

TEST_F(MonitorSafeToOperate, contactorDeclaredUnsafeWhenLastTemperatureCriticallyHigh)
{
   EXPECT_CALL(contactor, setSafeToOperate(false));

   monitor.sink(goodPackTemperatures()
                .setTemperature(PackTemperatures::NUM_SENSORS - 1, CRITICALLY_HIGH_TEMPERATURE));
}

TEST_F(MonitorSafeToOperate, contactorDeclaredUnsafeWhenSomeTemperatureInTheMiddleCriticallyLow)
{
   EXPECT_CALL(contactor, setSafeToOperate(false));

   monitor.sink(goodPackTemperatures()
                .setTemperature(PackTemperatures::NUM_SENSORS / 2, CRITICALLY_LOW_TEMPERATURE));
}

TEST_F(MonitorConstructed, valuesDefaultToNaN)
{
   EXPECT_TRUE(isnan(monitor.getSocPercent()));
   EXPECT_TRUE(isnan(monitor.getSohPercent()));
   EXPECT_TRUE(isnan(monitor.getEnergyRemainingKwh()));
   EXPECT_TRUE(isnan(monitor.getCapacityKwh()));
   EXPECT_TRUE(isnan(monitor.getCurrent()));
   EXPECT_TRUE(isnan(monitor.getVoltage()));
}

TEST_F(MonitorConstructed, socCanBeSet)
{
   monitor.sink(BatteryState().setSocPercent(12.3));
   EXPECT_THAT(monitor.getSocPercent(), FloatEq(12.3));
}

TEST_F(MonitorConstructed, sohCanBeSet)
{
   monitor.sink(BatteryState().setHealthPercent(32.1));
   EXPECT_THAT(monitor.getSohPercent(), FloatEq(32.1));
}

TEST_F(MonitorConstructed, capacity24kWhAtSoh100pc)
{
   monitor.sink(BatteryState().setHealthPercent(100));
   EXPECT_THAT(monitor.getCapacityKwh(), FloatEq(24));   
}

TEST_F(MonitorConstructed, capacity2p4kWhAtSoh25pc)
{
   monitor.sink(BatteryState().setHealthPercent(10));
   EXPECT_THAT(monitor.getCapacityKwh(), FloatEq(2.4));   
}

TEST_F(MonitorConstructed, energyRemaining24kWhAtSoh100pcSoc100pc)
{
   monitor.sink(BatteryState().setHealthPercent(100)
                              .setSocPercent(100));
   EXPECT_THAT(monitor.getEnergyRemainingKwh(), FloatEq(24));   
}

TEST_F(MonitorConstructed, energyRemaining12kWhAtSoh50pcSoc100pc)
{
   monitor.sink(BatteryState().setHealthPercent(50)
                              .setSocPercent(100));
   EXPECT_THAT(monitor.getEnergyRemainingKwh(), FloatEq(12));   
}

TEST_F(MonitorConstructed, energyRemaining1p2kWhAtSoh50pcSoc10pc)
{
   monitor.sink(BatteryState().setHealthPercent(50)
                              .setSocPercent(10));
   EXPECT_THAT(monitor.getEnergyRemainingKwh(), FloatEq(1.2));   
}

TEST_F(MonitorConstructed, current)
{
   monitor.sink(BatteryStatus().setCurrent(-12.3));
   EXPECT_THAT(monitor.getCurrent(), FloatEq(-12.3));
}

TEST_F(MonitorConstructed, voltage)
{
   monitor.sink(BatteryStatus().setVoltage(231.9));
   EXPECT_THAT(monitor.getVoltage(), FloatEq(231.9));
}

TEST_F(MonitorConstructed, averageTemperature)
{
   monitor.sink(PackTemperatures().setTemperature(0, 20.2)
                                  .setTemperature(1, -20.2)
                                  .setTemperature(2, -5)
                                  .setTemperature(3,  11));
   EXPECT_THAT(monitor.getTemperature(), FloatEq(1.5));
}

TEST_F(MonitorConstructed, averageTemperatureOneSensorMissing)
{
   monitor.sink(PackTemperatures().setTemperature(0, 20.2)
                                  .setTemperature(1, -20.2)
                                  .setTemperature(2, -6)
                                  .setTemperature(3, NAN));
   EXPECT_THAT(monitor.getTemperature(), FloatEq(-2));
}

TEST_F(MonitorConstructed, averageTemperatureNanIfAllSensorsMissing)
{
   monitor.sink(PackTemperatures().setTemperature(0, NAN)
                                  .setTemperature(1, NAN)
                                  .setTemperature(2, NAN)
                                  .setTemperature(3, NAN));
   EXPECT_TRUE(isnan(monitor.getTemperature()));
}


class MonitorLimits: public MonitorConstructed {};

TEST_F(MonitorLimits, currentLimitsZeroInitially)
{
   EXPECT_THAT(monitor.getChargeCurrentLimit(), 0);
   EXPECT_THAT(monitor.getDischargeCurrentLimit(), 0);
}

TEST_F(MonitorLimits, currentUnlimitedAtNominalValues)
{
   monitor.sink(goodPackTemperatures());
   monitor.sink(goodCellVoltageRange());
   
   EXPECT_THAT(monitor.getChargeCurrentLimit(), NOMINAL_CURRENT_LIMIT);
   EXPECT_THAT(monitor.getDischargeCurrentLimit(), NOMINAL_CURRENT_LIMIT);
}

TEST_F(MonitorLimits, currentLimitedWhenTemperatureAtHighWarnLimit)
{
   // We have 10 degrees from warn to critical, and 1 degree resolution,
   // so at warning level we're at 10/11 of the current
   monitor.sink(goodPackTemperatures().setTemperature(0, WARN_HIGH_TEMPERATURE));
   monitor.sink(goodCellVoltageRange());
   
   EXPECT_THAT(monitor.getChargeCurrentLimit(), FloatEq(NOMINAL_CURRENT_LIMIT*10/11));
   EXPECT_THAT(monitor.getDischargeCurrentLimit(), FloatEq(NOMINAL_CURRENT_LIMIT*10/11));
}

TEST_F(MonitorLimits, currentLimitedWhenTemperatureJustBelowCriticallyHighLimit)
{
   // We have 10 degrees from warn to critical, and 1 degree resolution,
   // so just below critical level we're at 1/11 of the current
   monitor.sink(goodPackTemperatures().setTemperature(0, CRITICALLY_HIGH_TEMPERATURE - 1));
   monitor.sink(goodCellVoltageRange());
   
   EXPECT_THAT(monitor.getChargeCurrentLimit(), FloatEq(NOMINAL_CURRENT_LIMIT*1/11));
   EXPECT_THAT(monitor.getDischargeCurrentLimit(), FloatEq(NOMINAL_CURRENT_LIMIT*1/11));
}

TEST_F(MonitorLimits, currentLimitsZeroWhenTemperatureAtCriticallyHighLimit)
{
   monitor.sink(goodPackTemperatures().setTemperature(0, CRITICALLY_HIGH_TEMPERATURE));
   monitor.sink(goodCellVoltageRange());
   
   EXPECT_THAT(monitor.getChargeCurrentLimit(), 0);
   EXPECT_THAT(monitor.getDischargeCurrentLimit(), 0);
}

TEST_F(MonitorLimits, currentLimitedWhenTemperatureAtLowWarnLimit)
{
   // We have 3 degrees from warn to critical, and 1 degree resolution,
   // so at warning level we're at 3/4 of the current
   monitor.sink(goodPackTemperatures().setTemperature(0, WARN_LOW_TEMPERATURE));
   monitor.sink(goodCellVoltageRange());
   
   EXPECT_THAT(monitor.getChargeCurrentLimit(), FloatEq(NOMINAL_CURRENT_LIMIT*3/4));
   EXPECT_THAT(monitor.getDischargeCurrentLimit(), FloatEq(NOMINAL_CURRENT_LIMIT*3/4));
}

TEST_F(MonitorLimits, currentLimitedWhenTemperatureJustAboveCriticallyLowLimit)
{
   // We have 3 degrees from warn to critical, and 1 degree resolution,
   // so at warning level we're at 1/4 of the current
   monitor.sink(goodPackTemperatures().setTemperature(0, CRITICALLY_LOW_TEMPERATURE + 1));
   monitor.sink(goodCellVoltageRange());
   
   EXPECT_THAT(monitor.getChargeCurrentLimit(), FloatEq(NOMINAL_CURRENT_LIMIT*1/4));
   EXPECT_THAT(monitor.getDischargeCurrentLimit(), FloatEq(NOMINAL_CURRENT_LIMIT*1/4));
}

TEST_F(MonitorLimits, currentLimitsZeroWhenTemperatureAtCriticallyLowLimit)
{
   monitor.sink(goodPackTemperatures().setTemperature(0, CRITICALLY_LOW_TEMPERATURE));
   monitor.sink(goodCellVoltageRange());
   
   EXPECT_THAT(monitor.getChargeCurrentLimit(), 0);
   EXPECT_THAT(monitor.getDischargeCurrentLimit(), 0);
}


}
