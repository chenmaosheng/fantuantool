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
	COMMAND_GATEALLOCREQ = COMMAND_APPSTART+1,
	COMMAND_GATERELEASEREQ,
	COMMAND_REGIONBINDREQ,
};


struct LogicCommandGateAllocReq : public LogicCommandT<COMMAND_GATEALLOCREQ>
{
	LogicCommandGateAllocReq()
	{
		m_iLoginSessionId = 0;
		m_strAccountName[0] = _T('\0');
	}

	uint32 m_iLoginSessionId;
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
};

struct LogicCommandGateReleaseReq : public LogicCommandT<COMMAND_GATERELEASEREQ>
{
	LogicCommandGateReleaseReq()
	{
		m_iLoginSessionId = 0;
		m_strAccountName[0] = _T('\0');
	}

	uint32 m_iLoginSessionId;
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
};

struct LogicCommandRegionBindReq : public LogicCommandT<COMMAND_REGIONBINDREQ>
{
	LogicCommandRegionBindReq()
	{
		m_iSessionId = 0;
		m_iRegionServerId = 0;
	}

	uint32 m_iSessionId;
	uint8 m_iRegionServerId;
};

#endif