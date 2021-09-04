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

	if (frame.getByte(0) == ENTERSLEEP) {
		m_command = ENTERSLEEP;
	}
	else if (frame.getByte(0) == QUITSLEEP) {
		m_command = QUITSLEEP;
	}
	else return;

	setValid();
}

SleepAwakeCommand::Command SleepAwakeCommand::getCommand() const
{
   return m_command;
}

}
}
}
