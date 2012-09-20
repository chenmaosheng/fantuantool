#ifndef _H_LOGIC_COMMAND
#define _H_LOGIC_COMMAND

#include "common.h"

enum
{
	COMMAND_ONCONNECT,
	COMMAND_ONDISCONNECT,
	COMMAND_ONDATA,
};

class LogicCommand
{
public:
	LogicCommand(){}

public:
	int m_iCmdId;
};

template<uint16 iCmdId>
class LogicCommandT : public LogicCommand
{
public:
	const static uint16 CmdId = iCmdId;

	LogicCommandT()
	{
		m_iCmdId = CmdId;
	}
};

class LogicCommandOnConnect : public LogicCommandT<COMMAND_ONCONNECT>
{
public:
	LogicCommandOnConnect()
	{
		m_ConnId = NULL;
	}

public:
	ConnID m_ConnId;
};

class LogicCommandOnDisconnect : public LogicCommandT<COMMAND_ONDISCONNECT>
{
public:
	LogicCommandOnDisconnect()
	{
		m_ConnId = NULL;
	}

public:
	ConnID m_ConnId;
};

class LogicCommandOnData : public LogicCommandT<COMMAND_ONDATA>
{
public:
	LogicCommandOnData();
	~LogicCommandOnData();
	bool CopyData(uint16 iLen, const char* pData);
	
public:
	ConnID m_ConnId;
	uint16 m_iLen;
	char* m_pData;
};

#endif
