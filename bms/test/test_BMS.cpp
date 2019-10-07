#include <gmock/gmock.h>
#include "bms/BMS.hpp"
#include "can/DataFrame.hpp"
#include <algorithm>

const can::DataFrame TEST_FRAMES[]{
   // voltages
   can::DataFrame(0x6F2, {0x00, 0x88, 0xF4, 0x25, 0x1D, 0x49, 0x5F, 0xD2}),
   can::DataFrame(0x6F2, {0x01, 0x8D, 0xB4, 0x21, 0x2D, 0x49, 0x73, 0xD2}),
   can::DataFrame(0x6F2, {0x02, 0x94, 0xF4, 0x25, 0x5D, 0x49, 0x3F, 0xD2}),
   can::DataFrame(0x6F2, {0x03, 0x93, 0x74, 0x28, 0x8D, 0x49, 0x73, 0xD2}),
   can::DataFrame(0x6F2, {0x04, 0x95, 0x34, 0x24, 0xCD, 0x48, 0x5B, 0xD2}),
   can::DataFrame(0x6F2, {0x05, 0x8E, 0xF4, 0x24, 0xCD, 0x48, 0x0F, 0xD2}),
   can::DataFrame(0x6F2, {0x06, 0x8C, 0x34, 0x27, 0x3D, 0x49, 0x67, 0xD2}),
   can::DataFrame(0x6F2, {0x07, 0x93, 0xF4, 0x23, 0xED, 0x47, 0x2B, 0xD2}),
   can::DataFrame(0x6F2, {0x08, 0x83, 0x34, 0x22, 0xFD, 0x47, 0xF7, 0xD1}),
   can::DataFrame(0x6F2, {0x09, 0x7F, 0xB4, 0x23, 0x8D, 0x48, 0x27, 0xD2}),
   can::DataFrame(0x6F2, {0x0A, 0x82, 0xB4, 0x1F, 0x4D, 0x48, 0x53, 0xD2}),
   can::DataFrame(0x6F2, {0x0B, 0x8A, 0xB4, 0x24, 0xBD, 0x48, 0x17, 0xD2}),
   can::DataFrame(0x6F2, {0x0C, 0x93, 0xF4, 0x28, 0x1D, 0x4A, 0x8F, 0xD2}),
   can::DataFrame(0x6F2, {0x0D, 0xA1, 0xF4, 0x25, 0x7D, 0x49, 0x8F, 0xD2}),
   can::DataFrame(0x6F2, {0x0E, 0x99, 0x34, 0x28, 0x7D, 0x49, 0x5B, 0xD2}),
   can::DataFrame(0x6F2, {0x0F, 0x7F, 0xF4, 0x23, 0x9D, 0x48, 0x37, 0xD2}),
   can::DataFrame(0x6F2, {0x10, 0x83, 0x34, 0x20, 0x8D, 0x48, 0x5F, 0xD2}),
   can::DataFrame(0x6F2, {0x11, 0x92, 0xF4, 0x25, 0x1D, 0x49, 0x1B, 0xD2}),
   can::DataFrame(0x6F2, {0x12, 0x84, 0xF4, 0x24, 0xED, 0x48, 0x53, 0xD2}),
   can::DataFrame(0x6F2, {0x13, 0x89, 0xF4, 0x20, 0x8D, 0x48, 0x4F, 0xD2}),
   can::DataFrame(0x6F2, {0x14, 0x8E, 0xF4, 0x23, 0xDD, 0x48, 0x1B, 0xD2}),
   can::DataFrame(0x6F2, {0x15, 0x93, 0x74, 0x28, 0x8D, 0x49, 0x83, 0xD2}),
   can::DataFrame(0x6F2, {0x16, 0x95, 0x34, 0x25, 0x4D, 0x48, 0x53, 0xD2}),
   can::DataFrame(0x6F2, {0x17, 0x8E, 0xF4, 0x24, 0x9D, 0x48, 0x1F, 0xD2}),
   // temperatures           
   can::DataFrame(0x6F2, {0x18, 0xC0, 0x87, 0xEF, 0xC1, 0x7B, 0xC0, 0xFC}),
   can::DataFrame(0x6F2, {0x19, 0xB0, 0x87, 0xEE, 0xA1, 0x7B, 0x70, 0x1E}),
   can::DataFrame(0x6F2, {0x1A, 0xBA, 0xC7, 0xE7, 0xD1, 0x79, 0x34, 0x1E}),
   can::DataFrame(0x6F2, {0x1B, 0x9F, 0x87, 0xDF, 0x41, 0x7C, 0xA8, 0x1D}),
   can::DataFrame(0x6F2, {0x1C, 0xAF, 0x87, 0xDC, 0xE1, 0x79, 0x10, 0x1E}),
   can::DataFrame(0x6F2, {0x1D, 0xA7, 0xC7, 0xE1, 0x71, 0x7A, 0x50, 0x1E}),
   can::DataFrame(0x6F2, {0x1E, 0xB1, 0x07, 0xE5, 0xA1, 0x7B, 0xAC, 0x1E}),
   can::DataFrame(0x6F2, {0x1F, 0xAE, 0x47, 0xED, 0x91, 0x7B, 0xB4, 0x1E})
};

const float CELL_VOLTAGES[96] =
{
   4.101640, 4.106215, 4.104385, 4.106215, 4.103165, 4.101030,
   4.104690, 4.107739, 4.105300, 4.106215, 4.105605, 4.103775,
   4.104995, 4.109265, 4.106520, 4.107740, 4.105605, 4.104080,
   4.102860, 4.105910, 4.103470, 4.104995, 4.102860, 4.100115,
   4.102860, 4.107740, 4.104995, 4.106825, 4.104995, 4.103775,
   4.098590, 4.102250, 4.100115, 4.101640, 4.098895, 4.098285,
   4.098895, 4.103470, 4.101640, 4.101945, 4.099810, 4.098590,
   4.100420, 4.105300, 4.102250, 4.104690, 4.102555, 4.100725,
   4.104995, 4.109875, 4.109265, 4.109875, 4.109265, 4.106215,
   4.106215, 4.109875, 4.106825, 4.108960, 4.106215, 4.105910,
   4.098895, 4.103775, 4.101945, 4.103165, 4.100115, 4.099200,
   4.101640, 4.106215, 4.104690, 4.106215, 4.104385, 4.101030,
   4.100420, 4.104995, 4.103470, 4.105300, 4.101945, 4.100115,
   4.101640, 4.104995, 4.103470, 4.103775, 4.103165, 4.101030,
   4.104995, 4.109265, 4.106520, 4.108960, 4.105605, 4.105300,
   4.100420, 4.105300, 4.103470, 4.104995, 4.101945, 4.101335
};

const float TEMPERATURES[32] =
{
   24.204800, 24.180401,
   24.156000, -2.537600,
   24.009600, 24.131599,
   24.131599, 23.765600,
   24.131599, 23.802200,
   23.777800, 23.582600,
   23.802200, 23.399599,
   24.253599, 23.155600,
   23.997400, 23.253201,
   23.790001, 23.472799,
   23.899799, 23.509399,
   23.899799, 23.667999,
   24.021799, 23.667999,
   24.131599, 23.948601,
   23.985201, 24.070601,
   24.119400, 23.973000
};

float sumCells(BMS& bms, unsigned from = 0, unsigned to = 95)
{
   float r = 0;
   for (unsigned i = from; i <= to; i++)
   {
      r += bms.getCellVoltage(i);
   }
   return r;
}

class TestBMS: public testing::Test
{
public:
   BMS bms;
};

TEST_F(TestBMS, VoltageCells0to3)
{
   bms.receive(TEST_FRAMES[0]);

   EXPECT_FLOAT_EQ(sumCells(bms), bms.getVoltage());
   EXPECT_FLOAT_EQ(CELL_VOLTAGES[0], bms.getCellVoltage(0));
   EXPECT_FLOAT_EQ(CELL_VOLTAGES[1], bms.getCellVoltage(1));
   EXPECT_FLOAT_EQ(CELL_VOLTAGES[2], bms.getCellVoltage(2));
   EXPECT_FLOAT_EQ(CELL_VOLTAGES[3], bms.getCellVoltage(3));
   EXPECT_FLOAT_EQ(0.0, bms.getCellVoltage(4)) << "We haven't got that cell yet";
   
   EXPECT_FLOAT_EQ(0.0, bms.getTemperature(0));

   EXPECT_FLOAT_EQ(sumCells(bms, 0, 5), bms.getModuleVoltage(0));
   EXPECT_FLOAT_EQ(0.0, bms.getModuleVoltage(1)) << "We haven't got that module yet";
}

TEST_F(TestBMS, VoltageCells4to7)
{
   bms.receive(TEST_FRAMES[1]);

   EXPECT_FLOAT_EQ(sumCells(bms), bms.getVoltage());
   EXPECT_FLOAT_EQ( 0.0, bms.getCellVoltage(3)) << "We haven't got that cell yet";
   EXPECT_FLOAT_EQ(CELL_VOLTAGES[4], bms.getCellVoltage(4));
   EXPECT_FLOAT_EQ(CELL_VOLTAGES[5], bms.getCellVoltage(5));
   EXPECT_FLOAT_EQ(CELL_VOLTAGES[6], bms.getCellVoltage(6));
   EXPECT_FLOAT_EQ(CELL_VOLTAGES[7], bms.getCellVoltage(7));
   EXPECT_FLOAT_EQ( 0.0, bms.getCellVoltage(8)) << "We haven't got that cell yet";
   
   EXPECT_FLOAT_EQ( 0.0, bms.getTemperature(0)) << "We haven't got temperatures yet";

   EXPECT_FLOAT_EQ(sumCells(bms, 0,  5), bms.getModuleVoltage(0));
   EXPECT_FLOAT_EQ(sumCells(bms, 6, 11), bms.getModuleVoltage(1)) << "We haven't got that module yet";
   EXPECT_FLOAT_EQ( 0.0, bms.getModuleVoltage(2)) << "We haven't got that module yet";
}

TEST_F(TestBMS, TempSensors0to3)
{
   bms.receive(TEST_FRAMES[24]);

   EXPECT_FLOAT_EQ(0.0, bms.getVoltage()) << "We haven't got voltages yet";
   EXPECT_FLOAT_EQ(0.0, bms.getCellVoltage(0)) << "We haven't got voltages yet";
   
   EXPECT_FLOAT_EQ(TEMPERATURES[0], bms.getTemperature(0));
   EXPECT_FLOAT_EQ(TEMPERATURES[1], bms.getTemperature(1));
   EXPECT_FLOAT_EQ(TEMPERATURES[2], bms.getTemperature(2));
   EXPECT_FLOAT_EQ(TEMPERATURES[3], bms.getTemperature(3));
   EXPECT_FLOAT_EQ(0.0, bms.getTemperature(4)) << "We haven't got that sensor yet";

   EXPECT_FLOAT_EQ(0.0, bms.getModuleVoltage(0)) << "We haven't got voltages yet";
}

TEST_F(TestBMS, AllVoltagesAndTemperatures)
{
   for (unsigned k = 0; k < 32; k++)
   {
      bms.receive(TEST_FRAMES[k]);
   }

   EXPECT_FLOAT_EQ(sumCells(bms), bms.getVoltage());

   for (unsigned cell = 0; cell < 96; cell++)
   {
      SCOPED_TRACE(testing::Message("cell=") << cell);
      EXPECT_FLOAT_EQ(CELL_VOLTAGES[cell], bms.getCellVoltage(cell));
   }

   for (unsigned module = 0, cell = 0; module < 16; module++, cell += 6)
   {
      SCOPED_TRACE(testing::Message("module=") << module << ", cells=" << cell << ".." << cell+5);
      EXPECT_FLOAT_EQ(sumCells(bms, cell, cell+5), bms.getModuleVoltage(module));
   }

   for (unsigned sensor = 0; sensor < 32; sensor++)
   {
      SCOPED_TRACE(testing::Message("sensor=") << sensor);
      EXPECT_FLOAT_EQ(TEMPERATURES[sensor], bms.getTemperature(sensor));
   }
      
}
