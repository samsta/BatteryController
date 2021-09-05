#include "SleepAwakeCommand.hpp"
#include "Ids.hpp"

namespace can {
namespace messages {
namespace TSUN {

SleepAwakeCommand::SleepAwakeCommand(const DataFrame& frame):
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

SleepAwakeCommand::Command SleepAwakeCommand::getCommand() const
{
   return m_command;
}

}
}
}
