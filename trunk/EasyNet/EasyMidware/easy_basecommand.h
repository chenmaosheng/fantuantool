#ifndef _H_EASY_BASECOMMAND
#define _H_EASY_BASECOMMAND

#include "common.h"

enum
{
	COMMAND_ONCONNECT,		// receive connection
	COMMAND_ONDISCONNECT,	// receive disconnect
	COMMAND_ONDATA,			// receive data
	COMMAND_SENDDATA,		// send data
	COMMAND_SHUTDOWN,		// ready to shutdown
	COMMAND_DISCONNECT,		// force disconnect
	
	COMMAND_APPSTART = 1000,
};

struct LogicCommand
{
	int32 m_iCmdId;
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

struct LogicCommandSendData : public LogicCommandT<COMMAND_SENDDATA>
{
	LogicCommandSendData();
	~LogicCommandSendData();
	bool CopyData(uint16 iLen, const char* pData);

	uint32 m_iSessionId;
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
		m_iReason = 0;
	}

	uint32 m_iSessionId;
	uint8 m_iReason;
};

#endif