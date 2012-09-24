#ifndef _H_MASTER_LOGIC_COMMAND
#define _H_MASTER_LOGIC_COMMAND

#include "logic_command.h"

#define COMMAND_ONLOGINREQ 1001

struct LogicCommandOnLoginReq : public LogicCommandT<COMMAND_ONLOGINREQ>
{
	LogicCommandOnLoginReq()
	{
		m_iSessionId = 0;
		m_strAccountName[0] = '\0';
	}

	uint32 m_iSessionId;
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
};

#endif