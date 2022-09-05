#include <gmock/gmock.h>
#include <monitor/Nissan/LeafMonitor.hpp>
#include <limits>
#include "mocks/contactor/Contactor.hpp"
#include "can/messages/Nissan/PackTemperatures.hpp"
#include "can/messages/Nissan/CellVoltageRange.hpp"
#include "can/messages/Nissan/BatteryState.hpp"
#include "can/messages/Nissan/BatteryStatus.hpp"

using namespace testing;
using namespace monitor::Nissan;
using namespace can::messages::Nissan;

#if defined(__APPLE__)
	using ::isnan;
#else
	using std::isnan;
#endif

namespace {

const float ARBITRARY_SAFE_VOLTAGE = 3.5;
const float CRITICALLY_HIGH_VOLTAGE = 4.15;
//const float WARN_HIGH_VOLTAGE = 4.1;
//const float WARN_LOW_VOLTAGE = 3.3;
const float CRITICALLY_LOW_VOLTAGE = 3.0;
const float CRITICAL_SPREAD_VOLTAGE = 0.5;

const float ARBITRARY_SAFE_TEMPERATURE = 20.0;
const float CRITICALLY_HIGH_TEMPERATURE = 50;
const float CRITICALLY_LOW_TEMPERATURE = 2;
//const float WARN_HIGH_TEMPERATURE = 40;
//const float WARN_LOW_TEMPERATURE = 5;

//const float NOMINAL_CURRENT_LIMIT = 20;

const uint32_t CRIT_HIGH_VOLT(1 << 5);
const uint32_t CRIT_LOW_VOLT(1 << 4);
const uint32_t CRIT_SPREAD_VOLT(1 << 3);
const uint32_t CRIT_HIGH_TEMP(1 << 2);
const uint32_t CRIT_LOW_TEMP(1 << 1);
const uint32_t MAX_TEMP_MISSING(1 << 0);

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
   monitor.sink(BatteryStatus().setUsableSOC(12.3));
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
   monitor.sink(BatteryState().setHealthPercent(100));
   monitor.sink(BatteryStatus().setUsableSOC(100));
   EXPECT_THAT(monitor.getEnergyRemainingKwh(), FloatEq(24));   
}

TEST_F(MonitorConstructed, energyRemaining12kWhAtSoh50pcSoc100pc)
{
   monitor.sink(BatteryState().setHealthPercent(50));
   monitor.sink(BatteryStatus().setUsableSOC(100));
   EXPECT_THAT(monitor.getEnergyRemainingKwh(), FloatEq(12));   
}

TEST_F(MonitorConstructed, energyRemaining1p2kWhAtSoh50pcSoc10pc)
{
   monitor.sink(BatteryState().setHealthPercent(50));
   monitor.sink(BatteryStatus().setUsableSOC(10));
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


TEST_F(MonitorConstructed, dischargeCurrentTest1)
{
   monitor.sink(BatteryStatus().setVoltage(300.0));
   monitor.sink(BatteryPowerLimits().setDischargePowerLimit_kW(25.0));

   EXPECT_THAT(monitor.getDischargeCurrentLimit(), 1000.0 * 25.0/300.0);
}

TEST_F(MonitorConstructed, chargeCurrentTest1)
{
   monitor.sink(BatteryStatus().setVoltage(280.0));
   monitor.sink(BatteryPowerLimits().setChargePowerLimit_kW(15.0));

   EXPECT_THAT(monitor.getChargeCurrentLimit(), 1000.0 * 15.0/280.0);
}

TEST_F(MonitorConstructed, zeroInitialValue)
{
   EXPECT_THAT(monitor.getChargeCurrentLimit(), 0);
   EXPECT_THAT(monitor.getDischargeCurrentLimit(), 0);
}

TEST_F(MonitorSafeToOperate, contactorFlagTemperatureCriticallyHigh)
{
   monitor.sink(allPackTemperatures(CRITICALLY_HIGH_TEMPERATURE));

   EXPECT_TRUE(monitor.getContactorStatus() & CRIT_HIGH_TEMP);
}

TEST_F(MonitorSafeToOperate, contactorFlagTemperatureCriticallyLow)
{
   monitor.sink(allPackTemperatures(CRITICALLY_LOW_TEMPERATURE));

   EXPECT_TRUE(monitor.getContactorStatus() & CRIT_LOW_TEMP);
}

TEST_F(MonitorSafeToOperate, contactorFlagTemperatureSenseMissing)
{
   monitor.sink(allPackTemperatures(NAN));

   EXPECT_TRUE(monitor.getContactorStatus() & MAX_TEMP_MISSING);
}

TEST_F(MonitorSafeToOperate, contactorFlagVoltageRangeSpreadCritical)
{
   monitor.sink(CellVoltageRange()
                .setMin(ARBITRARY_SAFE_VOLTAGE)
                .setMax(ARBITRARY_SAFE_VOLTAGE + CRITICAL_SPREAD_VOLTAGE + TOLERANCE));

   EXPECT_TRUE(monitor.getContactorStatus() & CRIT_SPREAD_VOLT);
}

TEST_F(MonitorSafeToOperate, contactorFlagVoltageCriticallyLow)
{
   monitor.sink(CellVoltageRange()
                .setMin(CRITICALLY_LOW_VOLTAGE)
                .setMax(CRITICALLY_LOW_VOLTAGE + TOLERANCE));

   EXPECT_TRUE(monitor.getContactorStatus() & CRIT_LOW_VOLT);
}

TEST_F(MonitorSafeToOperate, contactorFlagVoltageCriticallyHigh)
{
   monitor.sink(CellVoltageRange()
                .setMin(CRITICALLY_HIGH_VOLTAGE - TOLERANCE)
                .setMax(CRITICALLY_HIGH_VOLTAGE));

   EXPECT_TRUE(monitor.getContactorStatus() & CRIT_HIGH_VOLT);
}
}
