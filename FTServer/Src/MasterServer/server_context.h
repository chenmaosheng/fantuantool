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
		m_dwLastReportTime = 0;
		m_bIsConnected = false;
	}

	uint8 m_iServerId;
	DWORD m_dwLastReportTime;
	bool m_bIsConnected;
	stdext::hash_map<uint32, MasterPlayerContext*> m_mPlayerContext;
};

struct GateServerContext
{
	GateServerContext()
	{
		m_iServerId = 0;
		m_dwLastReportTime = 0;
		m_bIsConnected = false;
		m_iSessionMax = 0;
		m_iSessionCount = 0;
	}

	uint8 m_iServerId;
	DWORD m_dwLastReportTime;
	bool m_bIsConnected;
	uint16 m_iSessionMax;
	uint16 m_iSessionCount;
	stdext::hash_map<uint32, MasterPlayerContext*> m_mPlayerContext;
};

struct RegionServerContext
{
	RegionServerContext()
	{
		m_iServerId = 0;
		m_iChannelId = 0;
	}

	uint8 m_iServerId;
	uint8 m_iChannelId;
};

struct ChannelContext
{
	ChannelContext()
	{
		m_iChannelId = 0;
		m_iAvatarMax = 0;
		m_strChannelName[0] = _T('\0');
		m_iInitialRegionServerId = 0;
	}

	uint8 m_iChannelId;
	uint16 m_iAvatarMax;
	TCHAR m_strChannelName[CHANNELNAME_MAX];
	uint8 m_iInitialRegionServerId;
	RegionServerContext m_arrayRegionContext[REGIONSERVER_MAX];
};

#endif