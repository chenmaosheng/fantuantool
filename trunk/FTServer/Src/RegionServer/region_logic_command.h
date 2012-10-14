/*****************************************************************************************
	filename:	region_logic_command.h
	created:	10/14/2012
	author:		chen
	purpose:	game command on region server

*****************************************************************************************/

#ifndef _H_REGION_LOGIC_COMMAND
#define _H_REGION_LOGIC_COMMAND

#include "logic_command.h"

enum
{
	COMMAND_ONREGIONALLOCREQ = COMMAND_APPSTART+1,		// receive alloc req
};

struct LogicCommandOnRegionAllocReq : public LogicCommandT<COMMAND_ONREGIONALLOCREQ>
{
	LogicCommandOnRegionAllocReq()
	{
		m_iSessionId = 0;
		m_iAvatarId = 0;
		m_strAvatarName[0] = _T('\0');
	}

	uint32 m_iSessionId;
	uint64 m_iAvatarId;
	TCHAR m_strAvatarName[AVATARNAME_MAX+1];
};

#endif