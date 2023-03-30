#ifndef CORE_RUNNINGAVG_HPP_
#define CORE_RUNNINGAVG_HPP_

class RunningAverage
{
public:
   RunningAverage();

   void dataPoint(float datapoint);
   float getAverage();
   void reset();
   
private:
   float m_average;
   unsigned m_count;

};

#endif /* CORE_RUNNINGAVG_HPP_ */
