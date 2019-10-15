#ifndef _CAN_MESSAGES_TESLA_DETAILEDCELLDATA_HPP
#define _CAN_MESSAGES_TESLA_DETAILEDCELLDATA_HPP

#include "logging/stream.hpp"

namespace can {

class DataFrame;

namespace messages {
namespace Tesla {

class DetailedCellData
{
public:
   DetailedCellData(const DataFrame& frame);
   DetailedCellData();

   bool valid() const;

   bool isVoltage() const;
   DetailedCellData& setIsVoltage();

   bool isTemperature() const { return not isVoltage(); }
   DetailedCellData& setIsTemperature();

   /*
    * Returns a voltage (if isVoltage()) or a temperature (if isTemperature()) for
    * index 0..3, NaN otherwise
    */
   float getValue(unsigned index) const;
   DetailedCellData& setValue(unsigned index, float value);

   /*
    * Returns the cell index (if isVoltage()) or temperature sensor index (if isTemperature())
    * for the first of the four values that can be accessed by getValue().
    */
   unsigned getBaseIndex() const;
   DetailedCellData& setBaseIndex(unsigned);

private:
   unsigned m_base_index;
   bool m_is_voltage;
   float m_values[4];
   bool m_valid;
};

logging::ostream& operator<<(logging::ostream&, const DetailedCellData&);

}
}
}
#endif // _CAN_MESSAGES_TESLA_DETAILEDCELLDATA_HPP
