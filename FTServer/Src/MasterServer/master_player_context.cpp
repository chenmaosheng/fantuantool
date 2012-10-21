#include "master_player_context.h"
#include "master_server_loop.h"
#include "master_server.h"
#include "master_server_config.h"

#include "gate_peer_send.h"
#include "session_peer_send.h"
#include "cache_peer_send.h"
#include "login_server_send.h"
#include "gate_server_send.h"
#include "region_peer_send.h"

#include "packet.h"

MasterServerLoop* MasterPlayerContext::m_pMainLoop = NULL;
uint16 MasterPlayerContext::m_iDelayTypeId = 0;
uint16 MasterPlayerContext::m_iDelayLen = 0;
char MasterPlayerContext::m_DelayBuf[MAX_INPUT_BUFFER] = {0};

MasterPlayerContext::MasterPlayerContext() :
m_StateMachine(PLAYER_STATE_NONE)
{
	Clear();
	_InitStateMachine();
}

MasterPlayerContext::~MasterPlayerContext()
{
	Clear();
}

void MasterPlayerContext::Clear()
{
	m_iSessionId = 0;
	m_strAccountName[0] = _T('\0');
	m_iGateServerId = 0;
	m_bFinalizing = false;
	m_StateMachine.SetCurrState(PLAYER_STATE_NONE);
	m_iAvatarCount = 0;
	m_strAvatarName[0] = _T('\0');
	m_iAvatarId = 0;
	m_iLastChannelId = 0;
	m_iRegionServerId = 0;
}

int32 MasterPlayerContext::DelaySendData(uint16 iTypeId, uint16 iLen, const char *pBuf)
{
	m_iDelayTypeId = iTypeId;
	m_iDelayLen = iLen;
	memcpy(m_DelayBuf, pBuf, iLen);

	return 0;
}

void MasterPlayerContext::OnLoginReq(uint32 iSessionId, const TCHAR* strAccountName)
{
	int32 iRet = 0;

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONLOGINREQ) != PLAYER_STATE_ONLOGINREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), strAccountName, iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}
	
	m_iSessionId = iSessionId;
	wcscpy_s(m_strAccountName, _countof(m_strAccountName), strAccountName);

	// notify gate
	iRet = m_pMainLoop->GateAllocReq();
	if (iRet < 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x No more gate session to allocate"), strAccountName, iSessionId);
		return;
	}

	m_iGateServerId = (uint8)iRet;
	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x Allocate a gate session on gate server id=%d"), strAccountName, iSessionId, iRet);

	iRet = GatePeerSend::GateAllocReq(g_pServer->GetPeerServer(m_iGateServerId), m_iSessionId, (uint16)wcslen(strAccountName)+1, strAccountName);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x Send gate allocate request failed"), strAccountName, iSessionId);
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_GATEALLOCREQ) != PLAYER_STATE_GATEALLOCREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), strAccountName, iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}
}

void MasterPlayerContext::GateAllocAck(uint8 iGateServerId, uint32 iGateSessionId)
{
	int32 iRet = 0;
	GateConfigItem* pConfigItem = NULL;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x Allocate gate success"), m_strAccountName, m_iSessionId);

	// means failed
	if (iGateServerId == 0)
	{
		if (m_StateMachine.StateTransition(PLAYER_EVENT_GATEALLOCFAILACK) != PLAYER_STATE_GATEALLOCFAILACK)
		{
			_ASSERT(false && _T("state error"));
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		}
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_GATEALLOCACK) != PLAYER_STATE_GATEALLOCACK)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	m_iGateServerId = iGateServerId;
	pConfigItem = g_pServerConfig->GetGateConfigItem(iGateServerId);
	if (!pConfigItem)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x Get gate server's config failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	// send gate server's ip and port to client
	iRet = LoginServerSend::LoginNtf(this, pConfigItem->m_iServerIP, pConfigItem->m_iServerPort);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x Send login notification failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	iRet = SessionPeerSend::PacketForward(g_pServer->m_pLoginServer, m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x Forward packet to login server failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	LOG_DBG(LOG_SERVER, _T("typeid=%d len=%d"), m_iDelayTypeId, m_iDelayLen);

	// set state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_GATEALLOCNTF) != PLAYER_STATE_GATEALLOCNTF)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	m_pMainLoop->LoginSession2GateSession(this, m_iSessionId, iGateSessionId);
}

void MasterPlayerContext::OnGateLoginReq()
{
	int32 iRet = 0;

	// check whether player will disconnect
	if (m_bFinalizing)
	{
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONGATELOGINREQ) != PLAYER_STATE_ONGATELOGINREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	iRet = CachePeerSend::OnLoginReq(g_pServer->m_pCacheServer, m_iSessionId, wcslen(m_strAccountName)+1, m_strAccountName);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x OnLoginReq failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x account logged in finished"), m_strAccountName, m_iSessionId);

	// check state again
	if (m_StateMachine.StateTransition(PLAYER_EVENT_CACHELOGINREQ) != PLAYER_STATE_CACHELOGINREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
	}

}

void MasterPlayerContext::OnSessionDisconnect()
{
	// todo:
	int32 iRet = 0;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x receive disconnect"), m_strAccountName, m_iSessionId);

	m_pMainLoop->ShutdownPlayer(this);
}

void MasterPlayerContext::OnAvatarListReq()
{
	int32 iRet = 0;

	// check whether player will disconnect
	if (m_bFinalizing)
	{
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONAVATARLISTREQ) != PLAYER_STATE_ONAVATARLISTREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x send avatar list request to cache server"), m_strAccountName, m_iSessionId);

	iRet = SessionPeerSend::PacketForward(g_pServer->m_pCacheServer, m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x PacketForward failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	LOG_DBG(LOG_SERVER, _T("typeid=%d len=%d"), m_iDelayTypeId, m_iDelayLen);

	// check state again
	if (m_StateMachine.StateTransition(PLAYER_EVENT_AVATARLISTREQ) != PLAYER_STATE_AVATARLISTREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}
}

void MasterPlayerContext::OnAvatarListAck(int32 iReturn, uint8 iAvatarCount, const prdAvatar* pAvatar)
{
	int32 iRet = 0;

	// check whether player will disconnect
	if (m_bFinalizing)
	{
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONAVATARLISTACK) != PLAYER_STATE_ONAVATARLISTACK)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x receive avatar list from cache server"), m_strAccountName, m_iSessionId);

	// check return value
	if (iReturn != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x return value is nonzero"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	m_iAvatarCount = iAvatarCount;
	memcpy(m_arrayAvatar, pAvatar, sizeof(prdAvatar)*iAvatarCount);

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x send avatar list to gate server"), m_strAccountName, m_iSessionId);
	iRet = SessionPeerSend::SendData(g_pServer->GetPeerServer(m_iGateServerId), m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x SendData failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
	}
	
	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_AVATARLISTACK) != PLAYER_STATE_AVATARLISTACK)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
	}
}

void MasterPlayerContext::OnAvatarCreateReq(prdAvatarCreateData &data)
{
	int32 iRet = 0;

	// check whether player will disconnect
	if (m_bFinalizing)
	{
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONAVATARCREATEREQ) != PLAYER_STATE_ONAVATARCREATEREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x receive avatar create, name=%s"), m_strAccountName, m_iSessionId, data.m_strAvatarName);

	// avatar is full
	if (m_iAvatarCount >= AVATARCOUNT_MAX)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x avatar is full"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	iRet = SessionPeerSend::PacketForward(g_pServer->m_pCacheServer, m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x PacketForward failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	LOG_DBG(LOG_SERVER, _T("typeid=%d len=%d"), m_iDelayTypeId, m_iDelayLen);

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_AVATARCREATEREQ) != PLAYER_STATE_AVATARCREATEREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
	}
}

void MasterPlayerContext::OnAvatarCreateAck(int32 iReturn, prdAvatar& newAvatar)
{
	int32 iRet = 0;

	// check whether player will disconnect
	if (m_bFinalizing)
	{
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONAVATARCREATEACK) != PLAYER_STATE_ONAVATARCREATEACK)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x receive new avatar from cache server"), m_strAccountName, m_iSessionId);

	// check return value
	if (iReturn != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x return value is nonzero"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	if (m_iAvatarCount >= AVATARCOUNT_MAX)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x avatar is full"), m_strAccountName, m_iSessionId);
		return;
	}

	memcpy(&m_arrayAvatar[m_iAvatarCount], &newAvatar, sizeof(prdAvatar));
	m_iAvatarCount++;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x name=%s send new avatar to gate server"), m_strAccountName, m_iSessionId, newAvatar.m_strAvatarName);
	iRet = SessionPeerSend::SendData(g_pServer->GetPeerServer(m_iGateServerId), m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x SendData failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
	}
	
	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_AVATARCREATEACK) != PLAYER_STATE_AVATARLISTACK)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
	}
}

void MasterPlayerContext::OnAvatarSelectReq(const TCHAR* strAvatarName)
{
	int32 iRet = 0;
	bool bExist = false;

	// check whether player will disconnect
	if (m_bFinalizing)
	{
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONAVATARSELECTREQ) != PLAYER_STATE_ONAVATARSELECTREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	// check if avatarname is valid
	for (uint8 i = 0; i < m_iAvatarCount; ++i)
	{
		if (wcscmp(m_arrayAvatar[i].m_strAvatarName, strAvatarName) == 0)
		{
			bExist = true;
			break;
		}
	}

	if (!bExist)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x can't find avatar name=%s"), m_strAccountName, m_iSessionId, strAvatarName);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x receive avatar select, name=%s"), m_strAccountName, m_iSessionId, strAvatarName);

	iRet = SessionPeerSend::PacketForward(g_pServer->m_pCacheServer, m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x PacketForward failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	LOG_DBG(LOG_SERVER, _T("typeid=%d len=%d"), m_iDelayTypeId, m_iDelayLen);

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_AVATARSELECTREQ) != PLAYER_STATE_AVATARSELECTREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
	}
}

void MasterPlayerContext::OnAvatarSelectAck(int32 iReturn, prdAvatarSelectData& data)
{
	int32 iRet = 0;
	
	// check whether player will disconnect
	if (m_bFinalizing)
	{
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONAVATARSELECTACK) != PLAYER_STATE_ONAVATARSELECTACK)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	if (iReturn != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x select avatar failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	// save current avatar info
	wcscpy_s(m_strAvatarName, _countof(m_strAvatarName), data.m_strAvatarName);
	m_iAvatarId = data.m_iAvatarId;
	m_iLastChannelId = data.m_iLastChannelId;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x name=%s send selected avatar to gate server"), m_strAccountName, m_iSessionId, data.m_strAvatarName);
	iRet = SessionPeerSend::SendData(g_pServer->GetPeerServer(m_iGateServerId), m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x SendData failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_AVATARSELECTACK) != PLAYER_STATE_AVATARSELECTACK)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	// send channel info
	iRet = m_pMainLoop->SendChannelList(this);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x send channel list failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_CHANNELLISTNTF) != PLAYER_STATE_CHANNELLISTNTF)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
	}
}

int32 MasterPlayerContext::SendChannelList(uint8 iChannelCount, ftdChannelData* arrayData)
{
	int32 iRet = 0;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x send channel list to client"), m_strAccountName, m_iSessionId);
	
	iRet = GateServerSend::ChannelListNtf(this, iChannelCount, arrayData);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x send channel list failed"), m_strAccountName, m_iSessionId);
		return -1;
	}

	iRet = SessionPeerSend::SendData(g_pServer->GetPeerServer(m_iGateServerId), m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x SendData failed"), m_strAccountName, m_iSessionId);
		return -1;
	}
	return 0;
}

void MasterPlayerContext::OnChannelSelectReq(const TCHAR* strChannelName)
{
	int32 iRet = 0;
	uint8 iChannelId = 0;
	uint8 iRegionServerId = 0;

	// check whether player will disconnect
	if (m_bFinalizing)
	{
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONCHANNELSELECTREQ) != PLAYER_STATE_ONCHANNELSELECTREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	// get channel id
	iChannelId = m_pMainLoop->GetChannelId(strChannelName);
	if (iChannelId == INVALID_CHANNEL_ID)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x channelName=%s not exists"), m_strAccountName, m_iSessionId, strChannelName);

		iRet = GateServerSend::ChannelSelectAck(this, -1);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x ChannelSelectAck failed"), m_strAccountName, m_iSessionId);
			return;
		}

		iRet = SessionPeerSend::SendData(g_pServer->GetPeerServer(m_iGateServerId), m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x SendData failed"), m_strAccountName, m_iSessionId);
			return;
		}

		return;
	}

	// check if channelId is last channel Id
	if (iChannelId != m_iLastChannelId)
	{
		// todo: clear player in last channel
	}

	// todo: it will depend on region which last time logged in
	iRegionServerId = m_pMainLoop->GetInitialRegionServerId(iChannelId);

	iRet = RegionPeerSend::RegionAllocReq(g_pServer->GetPeerServer(iRegionServerId), m_iSessionId, m_iAvatarId, wcslen(m_strAvatarName)+1, m_strAvatarName);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x RegionAllocReq failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}
	// update channel Id
	m_iLastChannelId = iChannelId;

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x Send RegionAllocReq"), m_strAccountName, m_iSessionId);

	if (m_StateMachine.StateTransition(PLAYER_EVENT_REGIONALLOCREQ) != PLAYER_STATE_REGIONALLOCREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}
}

void MasterPlayerContext::OnChannelLeaveReq()
{
	// todo:
}

void MasterPlayerContext::OnRegionAllocAck(uint8 iRegionServerId, int32 iReturn)
{
	int32 iRet = 0;

	// check whether player will disconnect
	if (m_bFinalizing)
	{
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONREGIONALLOCACK) != PLAYER_STATE_ONREGIONALLOCACK)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x region=%d Record player region info"), m_strAccountName, m_iSessionId, iRegionServerId);

	if (iReturn != 0)
	{
		// check state
		if (m_StateMachine.StateTransition(PLAYER_EVENT_ONREGIONALLOCFAILACK) != PLAYER_STATE_ONREGIONALLOCFAILACK)
		{
			_ASSERT(false && _T("state error"));
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		}

		iRet = GateServerSend::ChannelSelectAck(this, iReturn);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x ChannelSelectAck failed"), m_strAccountName, m_iSessionId);
			m_pMainLoop->ShutdownPlayer(this);
			return;
		}

		iRet = SessionPeerSend::SendData(g_pServer->GetPeerServer(m_iGateServerId), m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x SendData failed"), m_strAccountName, m_iSessionId);
			m_pMainLoop->ShutdownPlayer(this);
			return;
		}

		// check state
		if (m_StateMachine.StateTransition(PLAYER_EVENT_CHANNELSELECTACK) != PLAYER_STATE_CHANNELSELECTFAILACK)
		{
			_ASSERT(false && _T("state error"));
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		}

		// send channel info
		iRet = m_pMainLoop->SendChannelList(this);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x send channel list failed"), m_strAccountName, m_iSessionId);
			m_pMainLoop->ShutdownPlayer(this);
			return;
		}

		// check state
		if (m_StateMachine.StateTransition(PLAYER_EVENT_CHANNELLISTNTF) != PLAYER_STATE_CHANNELLISTNTF)
		{
			_ASSERT(false && _T("state error"));
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		}

		return;
	}

	m_iRegionServerId = iRegionServerId;

	// notify client
	iRet = GateServerSend::ChannelSelectAck(this, iReturn);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x ChannelSelectAck failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	iRet = SessionPeerSend::SendData(g_pServer->GetPeerServer(m_iGateServerId), m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x SendData failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_CHANNELSELECTACK) != PLAYER_STATE_CHANNELSELECTACK)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	// notify region server
	iRet = RegionPeerSend::RegionEnterReq(g_pServer->GetPeerServer(m_iRegionServerId), m_iSessionId);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x RegionEnterReq failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x Request enter region"), m_strAccountName, m_iSessionId);

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_REGIONENTERREQ) != PLAYER_STATE_REGIONENTERREQ)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	// add to region context
	m_pMainLoop->AddPlayerToRegionServerContext(this);
}

void MasterPlayerContext::OnRegionLeaveReq(uint8 iRegionServerId)
{
	int32 iRet = 0;

	// check state
	if (m_StateMachine.StateTransition(PLAYER_EVENT_ONREGIONLEAVEREQ) < 0)
	{
		_ASSERT(false && _T("state error"));
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	// delete player from region context
	m_pMainLoop->DeletePlayerFromRegionServerContext(this);

	// notify client leave channel
	iRet = GateServerSend::ChannelLeaveAck(this);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x ChannelLeaveAck failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	iRet = SessionPeerSend::SendData(g_pServer->GetPeerServer(m_iGateServerId), m_iSessionId, m_iDelayTypeId, m_iDelayLen, m_DelayBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x SendData failed"), m_strAccountName, m_iSessionId);
		m_pMainLoop->ShutdownPlayer(this);
		return;
	}

	switch(m_StateMachine.GetCurrState())
	{
	case PLAYER_STATE_ONCHANNELLEAVE_ONREGIONLEAVEREQ:
		{
			// send channel list again
			m_pMainLoop->SendChannelList(this);

			// check state
			if (m_StateMachine.StateTransition(PLAYER_EVENT_CHANNELLISTNTF) != PLAYER_STATE_CHANNELLISTNTF)
			{
				_ASSERT(false && _T("state error"));
				LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
			}

			return;
		}
		break;

	case PLAYER_STATE_ONCHANNELLEAVE_REGIONLEAVEACK:
		{
			// notify cache server
			iRet = CachePeerSend::OnLogoutReq(g_pServer->m_pCacheServer, m_iSessionId);
			if (iRet != 0)
			{
				LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x OnLogoutReq failed"), m_strAccountName, m_iSessionId);
				m_pMainLoop->ShutdownPlayer(this);
				return;
			}

			LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x OnLogoutReq"), m_strAccountName, m_iSessionId);

			// check state
			if (m_StateMachine.StateTransition(PLAYER_EVENT_LOGOUT) != PLAYER_STATE_LOGOUT)
			{
				_ASSERT(false && _T("state error"));
				LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
			}

			m_pMainLoop->ShutdownPlayer(this);
			return;
		}
		break;
	}
}






















int32 Sender::SendPacket(void* pClient, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	return ((MasterPlayerContext*)pClient)->DelaySendData(iTypeId, iLen, pBuf);
}

