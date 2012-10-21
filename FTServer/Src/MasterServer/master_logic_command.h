/*****************************************************************************************
	filename:	master_logic_command.h
	created:	09/27/2012
	author:		chen
	purpose:	game command on master server

*****************************************************************************************/

#ifndef _H_MASTER_LOGIC_COMMAND
#define _H_MASTER_LOGIC_COMMAND

#include "logic_command.h"

enum
{
	COMMAND_ONLOGINREPORT = COMMAND_APPSTART + 1,
	COMMAND_ONGATEREPORT,
	COMMAND_ONCACHEREPORT,
	COMMAND_ONREGIONREPORT,
	COMMAND_ONLOGINREQ,		// receive login request
	COMMAND_GATEALLOCACK,	// acknowledge gate session alloc request
	COMMAND_ONGATELOGINREQ,	// receive gate login request
	COMMAND_ONREGIONALLOCACK, // acknowledge region alloc
};

struct LogicCommandOnLoginReport : public LogicCommandT<COMMAND_ONLOGINREPORT>
{
	LogicCommandOnLoginReport()
	{
		m_iServerId = 0;
	}

	uint8 m_iServerId;
};

struct LogicCommandOnGateReport : public LogicCommandT<COMMAND_ONGATEREPORT>
{
	LogicCommandOnGateReport()
	{
		m_iServerId = 0;
		m_iSessionCount = 0;
	}

	uint8 m_iServerId;
	uint16 m_iSessionCount;
};

struct LogicCommandOnCacheReport : public LogicCommandT<COMMAND_ONCACHEREPORT>
{
	LogicCommandOnCacheReport()
	{
		m_iServerId = 0;
	}

	uint8 m_iServerId;
};

struct LogicCommandOnRegionReport : public LogicCommandT<COMMAND_ONREGIONREPORT>
{
	LogicCommandOnRegionReport()
	{
		m_iServerId = 0;
		m_iPlayerCount = 0;
	}

	uint8 m_iServerId;
	uint16 m_iPlayerCount;
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

struct LogicCommandOnGateLoginReq : public LogicCommandT<COMMAND_ONGATELOGINREQ>
{
	LogicCommandOnGateLoginReq()
	{
		m_iSessionId = 0;
		m_strAccountName[0] = _T('\0');
	}

	uint32 m_iSessionId;
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
};

struct LogicCommandGateAllocAck : public LogicCommandT<COMMAND_GATEALLOCACK>
{
	LogicCommandGateAllocAck()
	{
		m_iLoginSessionId = 0;
		m_iGateSessionId = 0;
		m_iServerId = 0;
		m_strAccountName[0] = _T('\0');
	}

	uint32 m_iLoginSessionId;
	uint32 m_iGateSessionId;
	uint8 m_iServerId;
	TCHAR m_strAccountName[ACCOUNTNAME_MAX + 1];
};

struct LogicCommandOnRegionAllocAck: public LogicCommandT<COMMAND_ONREGIONALLOCACK>
{
	LogicCommandOnRegionAllocAck()
	{
		m_iSessionId = 0;
		m_iServerId = 0;
		m_iReturn = 0;
	}

	uint32 m_iSessionId;
	uint8 m_iServerId;
	int32 m_iReturn;
};

#endif