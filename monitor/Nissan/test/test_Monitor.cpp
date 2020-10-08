#include <gmock/gmock.h>
#include <limits>
#include "monitor/Nissan/Monitor.hpp"
#include "contactor/Contactor.hpp"
#include "can/messages/Nissan/PackTemperatures.hpp"
#include "can/messages/Nissan/CellVoltageRange.hpp"

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

TEST_F(MonitorSafeToOperate, contactorDeclaredUnsafeWhenVoltageRangepreadCritical)
{
   EXPECT_CALL(contactor, setSafeToOperate(false));

   monitor.sink(CellVoltageRange()
                .setMin(ARBITRARY_SAFE_VOLTAGE)
                .setMax(ARBITRARY_SAFE_VOLTAGE + CRITICAL_SPREAD_VOLTAGE + TOLERANCE));
}

TEST_F(MonitorSafeToOperate, contactorNotDeclaredUnsafeWhenVoltageRangepreadJustBelowCritical)
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


}
