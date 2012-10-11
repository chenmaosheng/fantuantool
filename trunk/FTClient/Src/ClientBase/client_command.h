/*****************************************************************************************
	filename:	client_command.h
	created:	10/08/2012
	author:		chen
	purpose:	basic logic command definition

*****************************************************************************************/

#ifndef _H_CLIENT_COMMAND
#define _H_CLIENT_COMMAND

#include "client_common.h"
#include "memory_object.h"

enum
{
	COMMAND_ONCONNECT,		// receive connection
	COMMAND_ONDISCONNECT,	// receive disconnect
	COMMAND_ONDATA,			// receive data

	COMMAND_FROMAPP_START,
};

struct ClientCommand : public MemoryObject 
{
	int m_iCmdId;
};

template<uint16 iCmdId>
struct ClientCommandT : public ClientCommand
{
	const static uint16 CmdId = iCmdId;

	ClientCommandT()
	{
		m_iCmdId = CmdId;
	}
};

struct ClientCommandOnConnect : public ClientCommandT<COMMAND_ONCONNECT>
{
	ClientCommandOnConnect()
	{
		m_ConnId = NULL;
	}

	ConnID m_ConnId;
};

struct ClientCommandOnDisconnect : public ClientCommandT<COMMAND_ONDISCONNECT>
{
	ClientCommandOnDisconnect()
	{
		m_ConnId = NULL;
	}

	ConnID m_ConnId;
};

struct ClientCommandOnData : public ClientCommandT<COMMAND_ONDATA>
{
	ClientCommandOnData();
	~ClientCommandOnData();
	bool CopyData(uint16 iLen, const char* pData);

	ConnID m_ConnId;
	uint16 m_iLen;
	char* m_pData;
};



#endif
