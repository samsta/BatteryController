#include "AvgMinMax.hpp"

AvgMinMax::AvgMinMax():
   m_count(0),
   m_average(0),
   m_min(0),
   m_max(0)
{}

void AvgMinMax::dataPoint(float datapoint)
{
   if (m_count == 0) {
      m_average = datapoint;
      m_min = datapoint;
      m_max = datapoint;
   }
   else {
      // new_average = (old_average * (n-1) + new_value) / n
      m_average = ((m_average * m_count) + datapoint) / (m_count + 1);
      m_count++;
      if (datapoint > m_max) m_max = datapoint;
      if (datapoint < m_min) m_min = datapoint;
   }
}

float AvgMinMax::getAverage()
{
   return m_average;
}

float AvgMinMax::getMin()
{
   return m_min;
}

float AvgMinMax::getMax()
{
   return m_max;
}

void AvgMinMax::reset()
{
   m_count = 0;
   m_average = 0;
}
