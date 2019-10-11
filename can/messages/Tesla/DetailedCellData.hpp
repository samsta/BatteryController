#ifndef _CAN_MESSAGES_TESLA_DETAILEDCELLDATA_HPP
#define _CAN_MESSAGES_TESLA_DETAILEDCELLDATA_HPP

namespace can {

class DataFrame;

namespace messages {
namespace Tesla {

class DetailedCellData
{
public:
   DetailedCellData(const DataFrame& frame);

   bool valid() const;

   bool isVoltage() const;
   bool isTemperature() const { return not isVoltage(); }

   /*
    * Returns a voltage (if isVoltage()) or a temperature (if isTemperature()) for
    * index 0..3, NaN otherwise
    */
   float getValue(unsigned index) const;

   /*
    * Returns the cell index (if isVoltage()) or temperature sensor index (if isTemperature())
    * for the first of the four values that can be accessed by getValue().
    */
   unsigned getBaseIndex() const;

private:
   unsigned m_base_index;
   bool m_is_voltage;
   float m_values[4];
   bool m_valid;
};

}
}
}
#endif // _CAN_MESSAGES_TESLA_DETAILEDCELLDATA_HPP
