/*****************************************************************************************
	filename:	server_helper.h
	created:	10/17/2012
	author:		chen
	purpose:	define some helper functions for server

*****************************************************************************************/

#ifndef _H_SERVER_HELPER
#define _H_SERVER_HELPER

#include "server_common.h"

// describe a list of sessions on gate server
struct GateServerSessionDesc
{
	GateServerSessionDesc();
	void Clear();

	PEER_SERVER m_pGateServer;
	uint16 m_iSessionCount;
	uint32 m_arraySessionId[BROADCAST_SESSION_MAX];
};

class BroadcastHelper
{
public:
	BroadcastHelper();
	void Clear();
	void SetGateServer(uint8 iServerId, PEER_SERVER pGateServer);
	void AddGateSession(uint32 iSessionId);
	void SendData(uint16 iTypeId, uint16 iLen, const char* pBuf);

private:
	GateServerSessionDesc m_arrayGateServerSessionDesc[SERVERCOUNT_MAX];
};

class DelaySendData
{
public:
	DelaySendData();
	void Save(uint16 iDelayTypeId, uint16 iDelayLen, const char* pDelayBuf);

public:
	uint16 m_iDelayTypeId;
	uint16 m_iDelayLen;
	char m_DelayBuf[MAX_INPUT_BUFFER];
};

#endif
