#ifndef _H_EASY_LOGIC_COMMAND
#define _H_EASY_LOGIC_COMMAND

#include "logic_command.h"

enum
{
	COMMAND_ONPINGREQ = COMMAND_APPSTART + 1,
};

struct LogicCommandOnPingReq : public LogicCommandT<COMMAND_ONPINGREQ>
{
	LogicCommandOnPingReq()
	{
		m_iSessionId = 0;
		m_iVersion = 0;
	}

	uint32 m_iSessionId;
	int8 m_iVersion;
};

#endif
