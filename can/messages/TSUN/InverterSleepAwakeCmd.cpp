#include "InverterSleepAwakeCmd.hpp"
#include "Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

InverterSleepAwakeCmd::InverterSleepAwakeCmd(const DataFrame& frame):
		Message(ID_INVERTER_SLEEP_AWAKE_COMMAND),
		m_command()
{
	if (frame.id() != id()) return;
	if (frame.size() != 8) return;

	switch (frame.getByte(0))
	{
	   case ENTER_SLEEP:
	   case QUIT_SLEEP:
	      m_command = Command(frame.getByte(0));
	      break;

	   default:
	      return;
	}

	setValid();
}

InverterSleepAwakeCmd::Command InverterSleepAwakeCmd::getCommand() const
{
   return m_command;
}

void InverterSleepAwakeCmd::toStream(logging::ostream& os) const
{
   os << "InverterSleepAwakeCmd: ";
   if (not valid())
   {
      os << "invalid";
   }
   else
   {
      os << "Command=";
      switch (getCommand())
      {
      case ENTER_SLEEP:
         os << "ENTER_SLEEP";
         break;
      case QUIT_SLEEP:
         os << "QUIT_SLEEP";
         break;
      default:
         os << "Unknown";
         break;
      }
   }
}


}
}
}
