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
	COMMAND_ONLOGINREQ = COMMAND_APPSTART+1,		// receive login request
	COMMAND_ONLOGOUTREQ,		// receive logout request
	COMMAND_ONREGIONENTERREQ, // receive region enter request
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

struct LogicCommandOnLogoutReq : public LogicCommandT<COMMAND_ONLOGOUTREQ>
{
	LogicCommandOnLogoutReq()
	{
		m_iSessionId = 0;
	}

	uint32 m_iSessionId;
};

struct LogicCommandOnRegionEnterReq : public LogicCommandT<COMMAND_ONREGIONENTERREQ>
{
	LogicCommandOnRegionEnterReq()
	{
		m_iSessionId = 0;
		m_iServerId = 0;
		m_strAvatarName[0] = _T('\0');
	}

	uint32 m_iSessionId;
	uint8 m_iServerId;
	TCHAR m_strAvatarName[AVATARNAME_MAX+1];
};

#endif