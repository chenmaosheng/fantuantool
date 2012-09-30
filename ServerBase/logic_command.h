/*****************************************************************************************
	filename:	logic_command.h
	created:	09/27/2012
	author:		chen
	purpose:	basic logic command definition

*****************************************************************************************/

#ifndef _H_LOGIC_COMMAND
#define _H_LOGIC_COMMAND

#include "server_common.h"
#include "memory_object.h"

enum
{
	COMMAND_ONCONNECT,		// receive connection
	COMMAND_ONDISCONNECT,	// receive disconnect
	COMMAND_ONDATA,			// receive data
	COMMAND_BROADCASTDATA,	// ready to broadcast
	COMMAND_SHUTDOWN,		// ready to shutdown
	COMMAND_DISCONNECT,		// force disconnect
	COMMAND_PACKETFORWARD,	// forward packet to another server
	COMMAND_ONSESSIONDISCONNECT, // receive session disconnect
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

struct LogicCommandPacketForward : public LogicCommandT<COMMAND_PACKETFORWARD>
{
	LogicCommandPacketForward();
	~LogicCommandPacketForward();

	bool CopyData(uint16 iLen, const char* pData);

	uint32 m_iSessionId;
	uint16 m_iTypeId;
	uint16 m_iLen;
	char* m_pData;
};

struct LogicCommandOnSessionDisconnect : public LogicCommandT<COMMAND_ONSESSIONDISCONNECT>
{
	LogicCommandOnSessionDisconnect()
	{
		m_iSessionId = 0;
	}

	uint32 m_iSessionId;
};

#endif
