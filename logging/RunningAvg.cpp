#include "RunningAvg.hpp"

RunningAverage::RunningAverage():
   m_count(0),
   m_average(0)
{}

void RunningAverage::dataPoint(float datapoint)
{
   if (m_count == 0) {
      m_average = datapoint;
   }
   else {
      // new_average = (old_average * (n-1) + new_value) / n
      m_average = ((m_average * m_count) + datapoint) / (m_count + 1);
      m_count++;
   }
}

float RunningAverage::getAverage()
{
   return m_average;
}

void RunningAverage::reset()
{
   m_count = 0;
   m_average = 0;
}
