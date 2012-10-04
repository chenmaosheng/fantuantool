/*****************************************************************************************
	filename:	login_logic_command.h
	created:	10/04/2012
	author:		chen
	purpose:	game command in login server

*****************************************************************************************/

#ifndef _H_LOGIN_LOGIC_COMMAND
#define _H_LOGIn_LOGIC_COMMAND

#include "logic_command.h"

enum
{
	COMMAND_ONLOGINFAILEDACK = 1001,
};

struct LogicCommandOnLoginFailedAck : public LogicCommandT<COMMAND_ONLOGINFAILEDACK>
{
	LogicCommandOnLoginFailedAck()
	{
		m_iSessionId = 0;
		m_iReason = 0;
	}

	uint32 m_iSessionId;
	int8 m_iReason;
};