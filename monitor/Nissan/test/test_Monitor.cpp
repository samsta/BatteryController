#include <gmock/gmock.h>
#include <limits>
#include "monitor/Nissan/Monitor.hpp"
#include "contactor/Contactor.hpp"
#include "can/messages/Nissan/CellVoltages.hpp"
#include "can/messages/Nissan/PackTemperatures.hpp"

using namespace testing;
using namespace monitor::Nissan;
using namespace can::messages::Nissan;

namespace {

const float ARBITRARY_SAFE_VOLTAGE = 3.8;
const float CRITICALLY_HIGH_VOLTAGE = 4.15;
const float CRITICALLY_LOW_VOLTAGE = 3.0;
const float CRITICAL_SPREAD_VOLTAGE = 0.1;

const float ARBITRARY_SAFE_TEMPERATURE = 20.0;
const float CRITICALLY_HIGH_TEMPERATURE = 50;
const float CRITICALLY_LOW_TEMPERATURE = 2;

const float TOLERANCE = 0.0001;




class TestContactor: public NiceMock<contactor::Contactor>
{
public:
   MOCK_METHOD1(setSafeToOperate, void(bool));
};

CellVoltages allCellVoltages(float voltage)
{
   CellVoltages voltages;
   for (unsigned k = 0; k < CellVoltages::NUM_CELLS; k++)
   {
      voltages.setVoltage(k, voltage);
   }
   return voltages;
}

CellVoltages goodCellVoltages()
{
   return allCellVoltages(ARBITRARY_SAFE_VOLTAGE);
}

PackTemperatures allPackTemperatures(float temperature)
{
   PackTemperatures temperatures;
   for (unsigned k = 0; k < PackTemperatures::NUM_PACKS; k++)
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
   TestContactor contactor;

   EXPECT_CALL(contactor, setSafeToOperate(false));

   Monitor monitor(contactor);
}

class MonitorConstructed: public Test
{
public:
   MonitorConstructed(): monitor(contactor)
   {
   }

   TestContactor contactor;
   Monitor monitor;
};

TEST_F(MonitorConstructed, contactorNotDeclaredSafeWithOnlyGoodCellVoltages)
{
   EXPECT_CALL(contactor, setSafeToOperate(true)).Times(0);

   monitor.process(goodCellVoltages());
}

TEST_F(MonitorConstructed, contactorNotDeclaredSafeWithOnlyGoodPackTemperatures)
{
   EXPECT_CALL(contactor, setSafeToOperate(true)).Times(0);

   monitor.process(goodPackTemperatures());
}

TEST_F(MonitorConstructed, contactorDeclaredSafeWithGoodCellVoltagesFollowedByGoodPackTemperatures)
{
   EXPECT_CALL(contactor, setSafeToOperate(true));

   monitor.process(goodCellVoltages());
   monitor.process(goodPackTemperatures());
}

TEST_F(MonitorConstructed, contactorDeclaredSafeWithGoodCellVoltagesFollowedByGoodPackTemperaturesWithOneMissingSensor)
{
   EXPECT_CALL(contactor, setSafeToOperate(true));

   monitor.process(goodCellVoltages());
   monitor.process(goodPackTemperatures()
                   .setTemperature(0, NAN));
}

TEST_F(MonitorConstructed, contactorNotDeclaredSafeWithGoodCellVoltagesFollowedByGoodPackTemperaturesWithTwoMissingSensors)
{
   EXPECT_CALL(contactor, setSafeToOperate(true)).Times(0);

   monitor.process(goodCellVoltages());
   monitor.process(goodPackTemperatures()
                   .setTemperature(0, NAN)
                   .setTemperature(1, NAN));
}


TEST_F(MonitorConstructed, contactorDeclaredSafeWithGoodPackTemperaturesFollowedByGoodCellVoltages)
{
   EXPECT_CALL(contactor, setSafeToOperate(true));

   monitor.process(goodPackTemperatures());
   monitor.process(goodCellVoltages());
}

TEST_F(MonitorConstructed, contactorNotDeclaredSafeWithGoodCellVoltagesAndInvalidPackTemperatures)
{
   EXPECT_CALL(contactor, setSafeToOperate(true)).Times(0);

   monitor.process(goodCellVoltages());
   monitor.process(PackTemperatures());
}

TEST_F(MonitorConstructed, contactorNotDeclaredSafeWithGoodPackTemperaturesAndInvalidCellVoltages)
{
   EXPECT_CALL(contactor, setSafeToOperate(true)).Times(0);

   monitor.process(goodPackTemperatures());
   monitor.process(CellVoltages());
}

class MonitorSafeToOperate: public MonitorConstructed
{
public:
   MonitorSafeToOperate()
   {
      monitor.process(goodPackTemperatures());
      monitor.process(goodCellVoltages());
   }
};

TEST_F(MonitorSafeToOperate, contactorNoAdditionalDeclarationWithGoodVoltages)
{
   EXPECT_CALL(contactor, setSafeToOperate(_)).Times(0);

   monitor.process(goodCellVoltages());
}

TEST_F(MonitorSafeToOperate, contactorDeclaredUnsafeWhenVoltageCriticallyHigh)
{
   EXPECT_CALL(contactor, setSafeToOperate(false));

   monitor.process(allCellVoltages(CRITICALLY_HIGH_VOLTAGE));
}

TEST_F(MonitorSafeToOperate, contactorNotDeclaredUnsafeWhenVoltageJustBelowCriticallyHigh)
{
   EXPECT_CALL(contactor, setSafeToOperate(false)).Times(0);

   monitor.process(allCellVoltages(CRITICALLY_HIGH_VOLTAGE - TOLERANCE));
}

TEST_F(MonitorSafeToOperate, contactorDeclaredUnsafeWhenVoltageCriticallyLow)
{
   EXPECT_CALL(contactor, setSafeToOperate(false));

   monitor.process(allCellVoltages(CRITICALLY_LOW_VOLTAGE));
}

TEST_F(MonitorSafeToOperate, contactorNotDeclaredUnsafeWhenVoltageJustAboveCriticallyLow)
{
   EXPECT_CALL(contactor, setSafeToOperate(false)).Times(0);

   monitor.process(allCellVoltages(CRITICALLY_LOW_VOLTAGE + TOLERANCE));
}

TEST_F(MonitorSafeToOperate, contactorDeclaredUnsafeWhenLastVoltageCriticallyHigh)
{
   EXPECT_CALL(contactor, setSafeToOperate(false));

   monitor.process(allCellVoltages(CRITICALLY_HIGH_VOLTAGE - TOLERANCE)
                   .setVoltage(CellVoltages::NUM_CELLS - 1, CRITICALLY_HIGH_VOLTAGE));
}

TEST_F(MonitorSafeToOperate, contactorDeclaredUnsafeWhenSomeVoltageInTheMiddleCriticallyLow)
{
   EXPECT_CALL(contactor, setSafeToOperate(false));

   monitor.process(allCellVoltages(CRITICALLY_LOW_VOLTAGE + TOLERANCE)
                   .setVoltage(CellVoltages::NUM_CELLS / 2, CRITICALLY_LOW_VOLTAGE));
}

TEST_F(MonitorSafeToOperate, contactorDeclaredUnsafeWhenVoltageSpreadCritical)
{
   EXPECT_CALL(contactor, setSafeToOperate(false));

   CellVoltages voltages = goodCellVoltages();
   voltages.setVoltage(0, ARBITRARY_SAFE_VOLTAGE + CRITICAL_SPREAD_VOLTAGE + TOLERANCE);
   monitor.process(voltages);
}

TEST_F(MonitorSafeToOperate, contactorNotDeclaredUnsafeWhenVoltageSpreadJustBelowCritical)
{
   EXPECT_CALL(contactor, setSafeToOperate(false)).Times(0);

   CellVoltages voltages = goodCellVoltages();
   voltages.setVoltage(0, ARBITRARY_SAFE_VOLTAGE + CRITICAL_SPREAD_VOLTAGE - TOLERANCE);
   monitor.process(voltages);
}

TEST_F(MonitorSafeToOperate, contactorNoAdditionalDeclarationWithGoodTemperatures)
{
   EXPECT_CALL(contactor, setSafeToOperate(_)).Times(0);

   monitor.process(goodPackTemperatures());
}

TEST_F(MonitorSafeToOperate, contactorDeclaredUnsafeWhenTemperatureCriticallyHigh)
{
   EXPECT_CALL(contactor, setSafeToOperate(false));

   monitor.process(allPackTemperatures(CRITICALLY_HIGH_TEMPERATURE));
}

TEST_F(MonitorSafeToOperate, contactorNotDeclaredUnsafeWhenTemperatureJustBelowCriticallyHigh)
{
   EXPECT_CALL(contactor, setSafeToOperate(false)).Times(0);

   monitor.process(allPackTemperatures(CRITICALLY_HIGH_TEMPERATURE - TOLERANCE));
}

TEST_F(MonitorSafeToOperate, contactorDeclaredUnsafeWhenTemperatureCriticallyLow)
{
   EXPECT_CALL(contactor, setSafeToOperate(false));

   monitor.process(allPackTemperatures(CRITICALLY_LOW_TEMPERATURE));
}


TEST_F(MonitorSafeToOperate, contactorNotDeclaredUnsafeWhenTemperatureJustAboveCriticallyLow)
{
   EXPECT_CALL(contactor, setSafeToOperate(false)).Times(0);

   monitor.process(allPackTemperatures(CRITICALLY_LOW_TEMPERATURE + TOLERANCE));
}

TEST_F(MonitorSafeToOperate, contactorDeclaredUnsafeWhenLastTemperatureCriticallyHigh)
{
   EXPECT_CALL(contactor, setSafeToOperate(false));

   monitor.process(goodPackTemperatures()
                   .setTemperature(PackTemperatures::NUM_PACKS - 1, CRITICALLY_HIGH_TEMPERATURE));
}

TEST_F(MonitorSafeToOperate, contactorDeclaredUnsafeWhenSomeTemperatureInTheMiddleCriticallyLow)
{
   EXPECT_CALL(contactor, setSafeToOperate(false));

   monitor.process(goodPackTemperatures()
         .setTemperature(PackTemperatures::NUM_PACKS / 2, CRITICALLY_LOW_TEMPERATURE));
}


}
