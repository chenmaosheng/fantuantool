/*****************************************************************************************
	filename:	server_context.h
	created:	10/02/2012
	author:		chen
	purpose:	record all other server's context

*****************************************************************************************/

#ifndef _H_SERVER_CONTEXT
#define _H_SERVER_CONTEXT

#include "server_common.h"
#include <hash_map>

class MasterPlayerContext;
struct LoginServerContext
{
	LoginServerContext()
	{
		m_iServerId = 0;
	}

	uint8 m_iServerId;
	stdext::hash_map<uint32, MasterPlayerContext*> m_mPlayerContext;
};

struct GateServerContext
{
	GateServerContext()
	{
		m_iServerId = 0;
	}

	uint8 m_iServerId;
	stdext::hash_map<uint32, MasterPlayerContext*> m_mPlayerContext;
};

#endif