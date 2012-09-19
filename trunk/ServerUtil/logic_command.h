#ifndef _H_LOGIC_COMMAND
#define _H_LOGIC_COMMAND

#include "common.h"

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

#endif
