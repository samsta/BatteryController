#ifndef LOGGING_AVGMINMAX_HPP_
#define LOGGING_AVGMINMAX_HPP_

class AvgMinMax
{
public:
   AvgMinMax();

   void dataPoint(float datapoint);
   float getAverage();
   float getMin();
   float getMax();
   void reset();
   
private:
   float m_average;
   float m_min;
   float m_max;
   unsigned m_count;

};

#endif /* LOGGING_AVGMINMAX_HPP_ */
