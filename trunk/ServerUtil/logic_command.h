#ifndef _H_LOGIC_COMMAND
#define _H_LOGIC_COMMAND

#include "common.h"

enum
{
	COMMAND_ONCONNECT,
	COMMAND_ONDISCONNECT,
	COMMAND_ONDATA,
};

template<uint16 iCmdId>
class LogicCommand
{
public:
	const static uint16 CmdId = iCmdId;

	LogicCommand()
	{
		m_iCmdId = CmdId;
	}

	virtual ~LogicCommand(){}

public:
	int		m_iCmdId;
};

class LogicCommandOnConnect : public LogicCommand<COMMAND_ONCONNECT>
{
public:
	LogicCommandOnConnect()
	{
		m_ConnId = NULL;
	}

public:
	ConnID m_ConnId;
};

class LogicCommandOnDisconnect : public LogicCommand<COMMAND_ONDISCONNECT>
{
public:
	LogicCommandOnDisconnect()
	{
		m_ConnId = NULL;
	}

public:
	ConnID m_ConnId;
};

class LogicCommandOnData : public LogicCommand<COMMAND_ONDATA>
{
public:
	LogicCommandOnData();
	~LogicCommandOnData();
	int32 CopyData(uint16 iLen, const char* pData);
	
public:
	ConnID m_ConnId;
	uint16 m_iLen;
	char* m_pData;
};

#endif
