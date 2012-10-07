/*****************************************************************************************
	filename:	cache_logic_command.h
	created:	10/05/2012
	author:		chen
	purpose:	game command on cache server

*****************************************************************************************/

#ifndef _H_CACHE_LOGIC_COMMAND
#define _H_CACHE_LOGIC_COMMAND

#include "logic_command.h"

enum
{
	COMMAND_ONLOGINREQ = 1001,		// receive login request
};

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

#endif