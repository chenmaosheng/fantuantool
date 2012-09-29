/*****************************************************************************************
	filename:	master_logic_command.h
	created:	09/27/2012
	author:		chen
	purpose:	game command on master server

*****************************************************************************************/

#ifndef _H_MASTER_LOGIC_COMMAND
#define _H_MASTER_LOGIC_COMMAND

#include "logic_command.h"

#define COMMAND_ONLOGINREQ 1001		// receive login request
#define COMMAND_GATEHOLDACK 1002	// acknowledge gate session hold request

struct LogicCommandOnLoginReq : public LogicCommandT<COMMAND_ONLOGINREQ>
{
	LogicCommandOnLoginReq()
	{
		m_iSessionId = 0;
		m_strAccountName[0] = _T('\0');
	}

	uint32 m_iSessionId;
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
};

struct LogicCommandGateHoldAck : public LogicCommandT<COMMAND_GATEHOLDACK>
{
	LogicCommandGateHoldAck()
	{
		m_iLoginSessionId = 0;
		m_iGateSessionId = 0;
		m_iServerId = 0;
		m_strAccountName[0] = _T('\0');
	}

	uint32 m_iLoginSessionId;
	uint32 m_iGateSessionId;
	uint16 m_iServerId;
	TCHAR m_strAccountName[ACCOUNTNAME_MAX + 1];
};

#endif