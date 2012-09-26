#ifndef _H_GATE_LOGIC_COMMAND
#define _H_GATE_LOGIC_COMMAND

#include "logic_command.h"

#define COMMAND_GATEHOLDREQ 1001

struct LogicCommandGateHoldReq : public LogicCommandT<COMMAND_GATEHOLDREQ>
{
	LogicCommandGateHoldReq()
	{
		m_iLoginSessionId = 0;
		m_strAccountName[0] = '\0';
	}

	uint32 m_iLoginSessionId;
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
};

#endif