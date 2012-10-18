#include "server_helper.h"
#include "session.h"
#include "..\..\..\FtCommon\Src\GenFiles\gate_peer_send.h"

GateServerSessionDesc::GateServerSessionDesc()
{
	m_pGateServer = NULL;
	Clear();
}

void GateServerSessionDesc::Clear()
{
	m_iSessionCount = 0;
	memset(m_arraySessionId, 0, sizeof(m_arraySessionId));
}

BroadcastHelper::BroadcastHelper()
{
	Clear();
}

void BroadcastHelper::Clear()
{
	for (uint32 i = 0; i < SERVERCOUNT_MAX; ++i)
	{
		m_arrayGateServerSessionDesc[i].Clear();
	}
}

void BroadcastHelper::SetGateServer(uint8 iServerId, PEER_SERVER pGateServer)
{
	if (iServerId >= SERVERCOUNT_MAX)
	{
		_ASSERT(false);
		return;
	}

	m_arrayGateServerSessionDesc[iServerId].m_pGateServer = pGateServer;
}

void BroadcastHelper::AddGateSession(uint32 iSessionId)
{
	uint8 iServerId = ((SessionId*)&iSessionId)->sValue_.serverId_;
	_ASSERT(iServerId < SERVERCOUNT_MAX);
	if (m_arrayGateServerSessionDesc[iServerId].m_iSessionCount >= BROADCAST_SESSION_MAX)
	{
		_ASSERT(false);
		return;
	}

	m_arrayGateServerSessionDesc[iServerId].m_arraySessionId[m_arrayGateServerSessionDesc[iServerId].m_iSessionCount++] = iSessionId;
}

void BroadcastHelper::SendData(uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	int32 iRet = 0;
	for (uint32 i = 0; i < SERVERCOUNT_MAX; ++i)
	{
		if (!m_arrayGateServerSessionDesc[i].m_iSessionCount)
		{
			continue;
		}

		iRet = GatePeerSend::BroadcastData(m_arrayGateServerSessionDesc[i].m_pGateServer, m_arrayGateServerSessionDesc[i].m_iSessionCount, 
			m_arrayGateServerSessionDesc[i].m_arraySessionId, iTypeId, iLen, pBuf);
		if (iRet != 0)
		{
			_ASSERT(false);
		}
	}

	Clear();
}