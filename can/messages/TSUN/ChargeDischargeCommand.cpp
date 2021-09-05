#include "ChargeDischargeCommand.hpp"
#include "Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

ChargeDischargeCommand::ChargeDischargeCommand(const DataFrame& frame):
      Message(ID_INVERTER_CHARGE_DISCHARGE_COMMAND),
      m_command(INVALID)
{
   if (frame.id() != id()) return;
   if (frame.size() != 8) return;

   // byte 0 or byte 1 should be AA, but not both
   if (frame.getByte(0) == 0xAA and frame.getByte(1) == 0x00)
   {
      m_command = CHARGE;
      setValid();
   }
   else if (frame.getByte(0) == 0x00 && frame.getByte(1) == 0xAA)
   {
      m_command = DISCHARGE;
      setValid();
   }

   return;

}

ChargeDischargeCommand::Command ChargeDischargeCommand::getCommand() const
{
   return m_command;
}

}
}
}
