#ifndef _H_LOGIC_COMMAND
#define _H_LOGIC_COMMAND

#include "server_common.h"
#include "memory_object.h"

enum
{
	COMMAND_ONCONNECT,
	COMMAND_ONDISCONNECT,
	COMMAND_ONDATA,
	COMMAND_BROADCASTDATA,
	COMMAND_SHUTDOWN,
	COMMAND_DISCONNECT,
};

struct LogicCommand : public MemoryObject 
{
	int m_iCmdId;
};

template<uint16 iCmdId>
struct LogicCommandT : public LogicCommand
{
	const static uint16 CmdId = iCmdId;

	LogicCommandT()
	{
		m_iCmdId = CmdId;
	}
};

struct LogicCommandOnConnect : public LogicCommandT<COMMAND_ONCONNECT>
{
	LogicCommandOnConnect()
	{
		m_ConnId = NULL;
	}

	ConnID m_ConnId;
};

struct LogicCommandOnDisconnect : public LogicCommandT<COMMAND_ONDISCONNECT>
{
	LogicCommandOnDisconnect()
	{
		m_ConnId = NULL;
	}

	ConnID m_ConnId;
};

struct LogicCommandOnData : public LogicCommandT<COMMAND_ONDATA>
{
	LogicCommandOnData();
	~LogicCommandOnData();
	bool CopyData(uint16 iLen, const char* pData);
	
	ConnID m_ConnId;
	uint16 m_iLen;
	char* m_pData;
};

struct LogicCommandBroadcastData : public LogicCommandT<COMMAND_BROADCASTDATA>
{
	LogicCommandBroadcastData();
	~LogicCommandBroadcastData();
	bool CopyData(uint16 iLen, const char* pData);

	ConnID m_ConnId;
	uint16 m_iTypeId;
	uint16 m_iLen;
	char* m_pData;
};

struct LogicCommandShutdown : public LogicCommandT<COMMAND_SHUTDOWN>
{
};

struct LogicCommandDisconnect : public LogicCommandT<COMMAND_DISCONNECT>
{
	LogicCommandDisconnect()
	{
		m_iSessionId = 0;
	}

	uint32 m_iSessionId;
};

#endif
