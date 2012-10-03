/*****************************************************************************************
	filename:	gate_logic_command.h
	created:	09/27/2012
	author:		chen
	purpose:	game command in gate server

*****************************************************************************************/

#ifndef _H_GATE_LOGIC_COMMAND
#define _H_GATE_LOGIC_COMMAND

#include "logic_command.h"

enum
{
	COMMAND_GATEALLOCREQ = 1001,
	COMMAND_GATERELEASEREQ,
};


struct LogicCommandGateAllocReq : public LogicCommandT<COMMAND_GATEALLOCREQ>
{
	LogicCommandGateAllocReq()
	{
		m_iLoginSessionId = 0;
		m_strAccountName[0] = '\0';
	}

	uint32 m_iLoginSessionId;
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
};

struct LogicCommandGateReleaseReq : public LogicCommandT<COMMAND_GATERELEASEREQ>
{
	LogicCommandGateReleaseReq()
	{
		m_iLoginSessionId = 0;
		m_strAccountName[0] = '\0';
	}

	uint32 m_iLoginSessionId;
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
};

#endif