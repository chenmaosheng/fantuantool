#ifndef _H_EASY_LOGIC_LogicCommand
#define _H_EASY_LOGIC_LogicCommand

#include "common.h"

enum
{
	COMMAND_ONCONNECT,              // receive connection
	COMMAND_ONDISCONNECT,			// receive disconnect
	COMMAND_ONDATA,                 // receive data
};

struct LogicCommand
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

#endif
