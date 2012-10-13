#include "gate_session.h"
#include "gate_server.h"

#include "gate_client_recv.h"
#include "session_peer_send.h"
#include "ftd_define.h"

// forward to master server
void GateClientRecv::AvatarListReq(void* pClient)
{
	int32 iRet = 0;
	GateSession* pSession = (GateSession*)pClient;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x AvatarListReq"), pSession->m_strAccountName, pSession->m_iSessionId);

	iRet = SessionPeerSend::PacketForward(g_pServer->m_pMasterServer, pSession->m_iSessionId, pSession->m_iDelayTypeId, pSession->m_iDelayLen, pSession->m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x PacketForward failed"), pSession->m_strAccountName, pSession->m_iSessionId);
		pSession->Disconnect();
	}
}

void GateClientRecv::AvatarCreateReq(void *pClient, const ftdAvatarCreateData& data)
{
	int32 iRet = 0;
	GateSession* pSession = (GateSession*)pClient;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x AvatarCreateReq"), pSession->m_strAccountName, pSession->m_iSessionId);

	iRet = SessionPeerSend::PacketForward(g_pServer->m_pMasterServer, pSession->m_iSessionId, pSession->m_iDelayTypeId, pSession->m_iDelayLen, pSession->m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x PacketForward failed"), pSession->m_strAccountName, pSession->m_iSessionId);
		pSession->Disconnect();
	}
}

void GateClientRecv::AvatarSelectReq(void *pClient, const char* strAvatarName)
{
	int32 iRet = 0;
	GateSession* pSession = (GateSession*)pClient;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x AvatarSelectReq"), pSession->m_strAccountName, pSession->m_iSessionId);

	iRet = SessionPeerSend::PacketForward(g_pServer->m_pMasterServer, pSession->m_iSessionId, pSession->m_iDelayTypeId, pSession->m_iDelayLen, pSession->m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x PacketForward failed"), pSession->m_strAccountName, pSession->m_iSessionId);
		pSession->Disconnect();
	}
}

void GateClientRecv::ChannelSelectReq(void *pClient, const char *strChannelName)
{
	int32 iRet = 0;
	GateSession* pSession = (GateSession*)pClient;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x ChannelSelectReq"), pSession->m_strAccountName, pSession->m_iSessionId);

	iRet = SessionPeerSend::PacketForward(g_pServer->m_pMasterServer, pSession->m_iSessionId, pSession->m_iDelayTypeId, pSession->m_iDelayLen, pSession->m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x PacketForward failed"), pSession->m_strAccountName, pSession->m_iSessionId);
		pSession->Disconnect();
	}
}

void GateClientRecv::ChannelLeaveReq(void* pClient)
{
	int32 iRet = 0;
	GateSession* pSession = (GateSession*)pClient;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x ChannelLeaveReq"), pSession->m_strAccountName, pSession->m_iSessionId);

	iRet = SessionPeerSend::PacketForward(g_pServer->m_pMasterServer, pSession->m_iSessionId, pSession->m_iDelayTypeId, pSession->m_iDelayLen, pSession->m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x PacketForward failed"), pSession->m_strAccountName, pSession->m_iSessionId);
		pSession->Disconnect();
	}
}