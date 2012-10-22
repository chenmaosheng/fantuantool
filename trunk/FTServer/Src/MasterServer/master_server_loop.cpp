#include "master_server_loop.h"
#include "master_server.h"
#include "master_server_config.h"
#include "master_logic_command.h"
#include "master_player_context.h"

#include "session_peer_send.h"
#include "login_peer_send.h"
#include "gate_peer_send.h"
#include "cache_peer_send.h"
#include "region_peer_send.h"
#include "session.h"

#include "packet.h"

MasterServerLoop::MasterServerLoop() :
m_iShutdownStatus(NOT_SHUTDOWN),
m_PlayerContextPool(5000) // todo:
{
	memset(&m_arrayGateServerContext, 0, sizeof(m_arrayGateServerContext));
	memset(&m_arrayChannelContext, 0, sizeof(m_arrayChannelContext));
	m_iChannelCount = 0;
}

int32 MasterServerLoop::Init()
{
	int32 iRet = 0;

	iRet = super::Init();
	if (iRet != 0)
	{
		return iRet;
	}

	MasterPlayerContext::m_pMainLoop = this;

	GateConfigItem* pConfig = NULL;
	for (std::map<uint8, GateConfigItem>::iterator mit = g_pServerConfig->m_mGateConfigItems.begin();
		mit != g_pServerConfig->m_mGateConfigItems.end(); ++mit)
	{
		pConfig = &(mit->second);
		if (pConfig->m_iServerId > SERVERCOUNT_MAX || m_arrayGateServerContext[pConfig->m_iServerId] != NULL)
		{
			LOG_ERR(LOG_SERVER, _T("gate config got some error"));
			continue;
		}

		m_arrayGateServerContext[pConfig->m_iServerId] = new GateServerContext;
		m_arrayGateServerContext[pConfig->m_iServerId]->m_iServerId = pConfig->m_iServerId;
		m_arrayGateServerContext[pConfig->m_iServerId]->m_iSessionMax = pConfig->m_iSessionMax;
	}

	// record channel count
	m_iChannelCount = (uint8)g_pServerConfig->GetChannelConfigItems().size();

	ChannelConfigItem* pChannelConfig = NULL;
	for (std::map<std::wstring, ChannelConfigItem>::iterator mit2 = g_pServerConfig->GetChannelConfigItems().begin();
		mit2 != g_pServerConfig->GetChannelConfigItems().end(); ++mit2)
	{
		pChannelConfig = &(mit2->second);
		if (pChannelConfig->m_iChannelId > CHANNEL_MAX || m_arrayChannelContext[pChannelConfig->m_iChannelId] != NULL)
		{
			LOG_ERR(LOG_SERVER, _T("channel config got some error"));
			continue;
		}

		m_arrayChannelContext[pChannelConfig->m_iChannelId] = new ChannelContext;
		m_arrayChannelContext[pChannelConfig->m_iChannelId]->m_iChannelId = pChannelConfig->m_iChannelId;
		m_arrayChannelContext[pChannelConfig->m_iChannelId]->m_iAvatarMax = pChannelConfig->m_iPlayerMax;
		wcscpy_s(m_arrayChannelContext[pChannelConfig->m_iChannelId]->m_strChannelName, CHANNELNAME_MAX+1, pChannelConfig->m_strChannelName);
		m_arrayChannelContext[pChannelConfig->m_iChannelId]->m_iInitialRegionServerId = pChannelConfig->m_iInitialRegionServerId;
		for (uint8 i = 0; i < SERVERCOUNT_MAX; ++i)
		{
			m_arrayChannelContext[pChannelConfig->m_iChannelId]->m_arrayRegionContext[i].m_iServerId = pChannelConfig->m_arrayRegionConfig[i].m_iServerId;
			m_arrayChannelContext[pChannelConfig->m_iChannelId]->m_arrayRegionContext[i].m_iChannelId = pChannelConfig->m_arrayRegionConfig[i].m_iChannelId;
			m_arrayChannelContext[pChannelConfig->m_iChannelId]->m_arrayRegionContext[i].m_iPlayerMax = pChannelConfig->m_arrayRegionConfig[i].m_iPlayerMax;
		}
	}

	return 0;
}

void MasterServerLoop::Destroy()
{
	for (int32 i = 0; i < SERVERCOUNT_MAX; ++i)
	{
		if (m_arrayGateServerContext[i])
		{
			SAFE_DELETE(m_arrayGateServerContext[i]);
		}
	}

	super::Destroy();
}

int32 MasterServerLoop::Start()
{
	int32 iRet = 0;

	iRet = super::Start();
	if (iRet != 0)
	{
		return iRet;
	}

	return 0;
}

bool MasterServerLoop::IsReadyForShutdown() const
{
	return m_iShutdownStatus == READY_FOR_SHUTDOWN;
}

void MasterServerLoop::LoginSession2GateSession(MasterPlayerContext* pPlayerContext, uint32 iLoginSessionId, uint32 iGateSessionId)
{
	stdext::hash_map<uint32, MasterPlayerContext*>::iterator mit = m_LoginServerContext.m_mPlayerContext.find(iLoginSessionId);
	if (mit != m_LoginServerContext.m_mPlayerContext.end())
	{
		m_LoginServerContext.m_mPlayerContext.erase(mit);
	}

	uint8 iServerId = ((SessionId*)&iGateSessionId)->sValue_.serverId_;

	GateServerContext* pContext = m_arrayGateServerContext[iServerId];
	if (!pContext)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%d serverId=%d gate config got some error"), iGateSessionId, iServerId);
		return;
	}

	pPlayerContext->m_iSessionId = iGateSessionId;
	pContext->m_mPlayerContext.insert(std::make_pair(iGateSessionId, pPlayerContext));

	stdext::hash_map<uint32, MasterPlayerContext*>::iterator mit2 = m_mPlayerContextBySessionId.find(iLoginSessionId);
	if (mit2 != m_mPlayerContextBySessionId.end())
	{
		m_mPlayerContextBySessionId.erase(mit2);
		m_mPlayerContextBySessionId.insert(std::make_pair(iGateSessionId, pPlayerContext));
	}
}

int32 MasterServerLoop::GateAllocReq()
{
	// todo: need a better policy
	GateServerContext* pContext = NULL;
	int32 iGateServerId = -1;

	for (int32 i = 0; i < SERVERCOUNT_MAX; ++i)
	{
		pContext = m_arrayGateServerContext[i];
		if (pContext &&
			pContext->m_iSessionMax > pContext->m_iSessionCount)
		{
			iGateServerId = pContext->m_iServerId;
			pContext->m_iSessionCount++;
			break;
		}
	}
	
	return iGateServerId;
}

void MasterServerLoop::ShutdownPlayer(MasterPlayerContext* pPlayerContext)
{
	int32 iRet = 0;
	bool bSendOnLoginFailedAck = false;
	bool bSendGateReleaseReq = false;
	bool bSendGateDisconnect = false;
	bool bSendCacheDisconnect = false;
	bool bSendRegionRelease = false;
	bool bSendRegionLeave = false;

	if (pPlayerContext->m_bFinalizing)
	{
		return;
	}

	switch (pPlayerContext->m_StateMachine.GetCurrState())
	{
	case PLAYER_STATE_ONLOGINREQ:
		bSendOnLoginFailedAck = true;
		break;

	case PLAYER_STATE_GATEALLOCREQ:
	case PLAYER_STATE_GATEALLOCACK:
		bSendOnLoginFailedAck = true;
		bSendGateReleaseReq = true;
		break;

	case PLAYER_STATE_GATEALLOCFAILACK:
		bSendOnLoginFailedAck = true;
		break;

	case PLAYER_STATE_GATEALLOCNTF:
		bSendGateReleaseReq = true;
		break;

	case PLAYER_STATE_ONGATELOGINREQ:
		bSendGateDisconnect = true;
		break;

	case PLAYER_STATE_CACHELOGINREQ:
	case PLAYER_STATE_ONAVATARLISTREQ:
	case PLAYER_STATE_AVATARLISTREQ:
	case PLAYER_STATE_ONAVATARLISTACK:
	case PLAYER_STATE_AVATARLISTACK:
	case PLAYER_STATE_ONAVATARCREATEREQ:
	case PLAYER_STATE_AVATARCREATEREQ:
	case PLAYER_STATE_ONAVATARCREATEACK:
	case PLAYER_STATE_ONAVATARSELECTREQ:
	case PLAYER_STATE_AVATARSELECTREQ:
	case PLAYER_STATE_ONAVATARSELECTACK:
	case PLAYER_STATE_AVATARSELECTACK:
	case PLAYER_STATE_CHANNELLISTNTF:
	case PLAYER_STATE_ONCHANNELSELECTREQ:
	case PLAYER_STATE_ONREGIONALLOCFAILACK:
	case PLAYER_STATE_CHANNELSELECTFAILACK:
	case PLAYER_STATE_ONCHANNELLEAVE_ONREGIONLEAVEREQ:
	case PLAYER_STATE_FINALIZING_ONREGIONLEAVEREQ:
		bSendGateDisconnect = true;
		bSendCacheDisconnect = true;
		break;

	case PLAYER_STATE_REGIONALLOCREQ:
	case PLAYER_STATE_ONREGIONALLOCACK:
	case PLAYER_STATE_CHANNELSELECTACK:
		bSendGateDisconnect = true;
		bSendCacheDisconnect = true;
		bSendRegionRelease = true;
		break;

	case PLAYER_STATE_REGIONENTERREQ:
		pPlayerContext->m_StateMachine.StateTransition(PLAYER_EVENT_FINALIZE);

		bSendGateDisconnect = true;
		bSendCacheDisconnect = true;
		bSendRegionLeave = true;
		break;

	case PLAYER_STATE_ONCHANNELLEAVEREQ:
	case PLAYER_STATE_FINALIZING:
		bSendGateDisconnect = true;
		bSendCacheDisconnect = true;
		bSendRegionLeave = true;
		break;

	case PLAYER_STATE_LOGOUT:
		bSendGateDisconnect = true;
		break;
	}

	if (bSendRegionRelease && !IsReadyForShutdown())
	{
		RegionPeerSend::RegionReleaseReq(g_pServer->GetPeerServer(pPlayerContext->m_iRegionServerId), pPlayerContext->m_iSessionId);
		DeletePlayerFromRegionServerContext(pPlayerContext);
	}

	if (bSendRegionLeave)
	{
		if (!IsReadyForShutdown())
		{
			iRet = RegionPeerSend::RegionLeaveReq(g_pServer->GetPeerServer(pPlayerContext->m_iRegionServerId), pPlayerContext->m_iSessionId);
			if (iRet != 0)
			{
				LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x RegionLeaveReq failed"), pPlayerContext->m_strAccountName, pPlayerContext->m_iSessionId);
			}
			else
			{
				// need leave region first, it will shutdown later
				pPlayerContext->m_StateMachine.StateTransition(PLAYER_EVENT_REGIONLEAVEACK);
				return;
			}
		}
	}

	if (bSendOnLoginFailedAck && !IsReadyForShutdown())
	{
		LoginPeerSend::OnLoginFailedAck(g_pServer->m_pLoginServer, pPlayerContext->m_iSessionId, 3);
	}

	if (bSendGateReleaseReq && !IsReadyForShutdown())
	{
		GatePeerSend::GateReleaseReq(g_pServer->GetPeerServer(pPlayerContext->m_iGateServerId), pPlayerContext->m_iSessionId, wcslen(pPlayerContext->m_strAccountName)+1, pPlayerContext->m_strAccountName);
	}

	if (bSendGateDisconnect && !IsReadyForShutdown())
	{
		SessionPeerSend::Disconnect(g_pServer->GetPeerServer(pPlayerContext->m_iGateServerId), pPlayerContext->m_iSessionId, 0);
	}

	if (bSendCacheDisconnect && !IsReadyForShutdown())
	{
		CachePeerSend::OnLogoutReq(g_pServer->m_pCacheServer, pPlayerContext->m_iSessionId);
	}

	AddPlayerToFinalizingQueue(pPlayerContext);
}

void MasterServerLoop::AddPlayerToFinalizingQueue(MasterPlayerContext* pPlayerContext)
{
	pPlayerContext->m_bFinalizing = true;
	m_PlayerFinalizingQueue.push(pPlayerContext);
}

void MasterServerLoop::DeletePlayer(MasterPlayerContext* pPlayerContext)
{
	bool bNeedDeleteLoginServerContext = false;
	bool bNeedDeleteGateServerContext = false;
	bool bNeedDeleteRegionServerContext = false;

	switch(pPlayerContext->m_StateMachine.GetCurrState())
	{
	case PLAYER_STATE_ONLOGINREQ:
	case PLAYER_STATE_GATEALLOCREQ:
	case PLAYER_STATE_GATEALLOCFAILACK:
		bNeedDeleteLoginServerContext = true;
		break;

	case PLAYER_STATE_GATEALLOCACK:
	case PLAYER_STATE_GATEALLOCNTF:
	case PLAYER_STATE_ONGATELOGINREQ:
	case PLAYER_STATE_CACHELOGINREQ:
	case PLAYER_STATE_ONAVATARLISTREQ:
	case PLAYER_STATE_AVATARLISTREQ:
	case PLAYER_STATE_ONAVATARLISTACK:
	case PLAYER_STATE_AVATARLISTACK:
	case PLAYER_STATE_ONAVATARCREATEREQ:
	case PLAYER_STATE_AVATARCREATEREQ:
	case PLAYER_STATE_ONAVATARCREATEACK:
	case PLAYER_STATE_ONAVATARSELECTREQ:
	case PLAYER_STATE_AVATARSELECTREQ:
	case PLAYER_STATE_ONAVATARSELECTACK:
	case PLAYER_STATE_AVATARSELECTACK:
	case PLAYER_STATE_CHANNELLISTNTF:
	case PLAYER_STATE_ONCHANNELSELECTREQ:
		bNeedDeleteGateServerContext = true;
		break;

	case PLAYER_STATE_ONREGIONALLOCFAILACK:
	case PLAYER_STATE_CHANNELSELECTFAILACK:
	case PLAYER_STATE_ONCHANNELLEAVE_ONREGIONLEAVEREQ:
	case PLAYER_STATE_ONCHANNELLEAVE_REGIONLEAVEACK:
	case PLAYER_STATE_FINALIZING_ONREGIONLEAVEREQ:
	case PLAYER_STATE_REGIONALLOCREQ:
	case PLAYER_STATE_ONREGIONALLOCACK:
	case PLAYER_STATE_CHANNELSELECTACK:
	case PLAYER_STATE_REGIONENTERREQ:
	case PLAYER_STATE_ONCHANNELLEAVEREQ:
	case PLAYER_STATE_FINALIZING:
	case PLAYER_STATE_FINALIZING_REGIONLEAVEACK:
	case PLAYER_STATE_LOGOUT:
		bNeedDeleteGateServerContext = true;
		bNeedDeleteRegionServerContext = true;

	default:
		break;
	}

	if (bNeedDeleteLoginServerContext)
	{
		DeletePlayerFromLoginServerContext(pPlayerContext);
	}

	if (bNeedDeleteGateServerContext)
	{
		DeletePlayerFromGateServerContext(pPlayerContext);
	}

	if (bNeedDeleteRegionServerContext)
	{
		DeletePlayerFromRegionServerContext(pPlayerContext);
	}

	stdext::hash_map<std::wstring, MasterPlayerContext*>::iterator mit = m_mPlayerContextByName.find(pPlayerContext->m_strAccountName);
	if (mit != m_mPlayerContextByName.end())
	{
		m_mPlayerContextByName.erase(mit);
	}

	stdext::hash_map<uint32, MasterPlayerContext*>::iterator mit2 = m_mPlayerContextBySessionId.find(pPlayerContext->m_iSessionId);
	if (mit2 != m_mPlayerContextBySessionId.end())
	{
		m_mPlayerContextBySessionId.erase(mit2);
	}

	LOG_STT(LOG_SERVER, _T("acc=%s sid=%08x Delete player"), pPlayerContext->m_strAccountName, pPlayerContext->m_iSessionId);
	pPlayerContext->Clear();

	m_PlayerContextPool.Free(pPlayerContext);
}

void MasterServerLoop::DeletePlayerFromLoginServerContext(MasterPlayerContext* pPlayerContext)
{
	stdext::hash_map<uint32, MasterPlayerContext*>::iterator mit = m_LoginServerContext.m_mPlayerContext.find(pPlayerContext->m_iSessionId);
	if (mit != m_LoginServerContext.m_mPlayerContext.end())
	{
		m_LoginServerContext.m_mPlayerContext.erase(mit);
	}
}

void MasterServerLoop::DeletePlayerFromGateServerContext(MasterPlayerContext* pPlayerContext)
{
	if (pPlayerContext->m_iGateServerId >= SERVERCOUNT_MAX)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x serverid=%d serverid invalid"), pPlayerContext->m_strAccountName, pPlayerContext->m_iSessionId, pPlayerContext->m_iGateServerId);
		return;
	}

	GateServerContext* pContext = m_arrayGateServerContext[pPlayerContext->m_iGateServerId];
	if (pContext)
	{
		stdext::hash_map<uint32, MasterPlayerContext*>::iterator mit = pContext->m_mPlayerContext.find(pPlayerContext->m_iSessionId);
		if (mit != pContext->m_mPlayerContext.end())
		{
			pContext->m_mPlayerContext.erase(mit);
		}
	}
}

void MasterServerLoop::AddPlayerToRegionServerContext(MasterPlayerContext* pPlayerContext)
{
	ChannelContext* pChannelContext = m_arrayChannelContext[pPlayerContext->m_iLastChannelId];
	if (pChannelContext)
	{
		pChannelContext->m_arrayRegionContext[pPlayerContext->m_iRegionServerId].m_mPlayerContext.insert(std::make_pair(pPlayerContext->m_iSessionId, pPlayerContext));
	}
}

void MasterServerLoop::DeletePlayerFromRegionServerContext(MasterPlayerContext* pPlayerContext)
{
	ChannelContext* pChannelContext = m_arrayChannelContext[pPlayerContext->m_iLastChannelId];
	if (pChannelContext)
	{
		RegionServerContext& regionContext = pChannelContext->m_arrayRegionContext[pPlayerContext->m_iRegionServerId];
		stdext::hash_map<uint32, MasterPlayerContext*>::iterator mit = regionContext.m_mPlayerContext.find(pPlayerContext->m_iSessionId);
		if (mit != regionContext.m_mPlayerContext.end())
		{
			regionContext.m_mPlayerContext.erase(mit);
		}
	}
}

int32 MasterServerLoop::SendChannelList(MasterPlayerContext* pPlayerContext)
{
	int32 iRet = 0;
	ftdChannelData arrayChannelData[CHANNEL_MAX];
	for (uint8 i = 0; i < m_iChannelCount; ++i)
	{
		iRet = WChar2Char(m_arrayChannelContext[i]->m_strChannelName, arrayChannelData[i].m_strChannelName, CHANNELNAME_MAX+1);
		if (iRet == 0)
		{
			return -1;
		}
		arrayChannelData[i].m_strChannelName[iRet] = '\0';
		arrayChannelData[i].m_iState = 0; // todo
	}

	return pPlayerContext->SendChannelList(m_iChannelCount, arrayChannelData);
}

uint8 MasterServerLoop::GetChannelId(const TCHAR *strChannelName)
{
	for (uint8 i = 0; i < m_iChannelCount; ++i)
	{
		ChannelContext* context = m_arrayChannelContext[i];
		if (context && wcscmp(context->m_strChannelName, strChannelName) == 0)
		{
			return context->m_iChannelId;
		}
	}

	return INVALID_CHANNEL_ID;
}

uint8 MasterServerLoop::GetInitialRegionServerId(uint8 iChannelId)
{
	for (uint8 i = 0; i < m_iChannelCount; ++i)
	{
		ChannelContext* context = m_arrayChannelContext[i];
		if (context && context->m_iChannelId == iChannelId)
		{
			return context->m_iInitialRegionServerId;
		}
	}

	return INVALID_SERVER_ID;
}

DWORD MasterServerLoop::_Loop()
{
	while (!m_PlayerFinalizingQueue.empty())
	{
		DeletePlayer(m_PlayerFinalizingQueue.back());
		m_PlayerFinalizingQueue.pop();
	}

	if (m_iShutdownStatus == START_SHUTDOWN)
	{
		if (m_mPlayerContextBySessionId.empty())
		{
			m_iShutdownStatus = READY_FOR_SHUTDOWN;
		}
	}

	return 100;
}

bool MasterServerLoop::_OnCommand(LogicCommand* pCommand)
{
	if (m_iShutdownStatus >= START_SHUTDOWN)
	{
		return true;
	}

	switch (pCommand->m_iCmdId)
	{
	case COMMAND_SHUTDOWN:
		_OnCommandShutdown();
		break;

	case COMMAND_ONLOGINREPORT:
		_OnCommandOnLoginReport((LogicCommandOnLoginReport*)pCommand);
		break;

	case COMMAND_ONGATEREPORT:
		_OnCommandOnGateReport((LogicCommandOnGateReport*)pCommand);
		break;

	case COMMAND_ONLOGINREQ:
		_OnCommandOnLoginReq((LogicCommandOnLoginReq*)pCommand);
		break;

	case COMMAND_GATEALLOCACK:
		_OnCommandGateAllocAck((LogicCommandGateAllocAck*)pCommand);
		break;

	case COMMAND_ONGATELOGINREQ:
		_OnCommandOnGateLoginReq((LogicCommandOnGateLoginReq*)pCommand);
		break;

	case COMMAND_ONREGIONALLOCACK:
		_OnCommandOnRegionAllocAck((LogicCommandOnRegionAllocAck*)pCommand);
		break;

	case COMMAND_ONREGIONLEAVEREQ:
		_OnCommandOnRegionLeaveReq((LogicCommandOnRegionLeaveReq*)pCommand);
		break;

	case COMMAND_ONSESSIONDISCONNECT:
		_OnCommandOnSessionDisconnect((LogicCommandOnSessionDisconnect*)pCommand);
		break;

	case COMMAND_PACKETFORWARD:
		_OnCommandPacketForward((LogicCommandPacketForward*)pCommand);
		break;

	default:
		break;
	}

	return true;
}

void MasterServerLoop::_OnCommandShutdown()
{
	m_iShutdownStatus = START_SHUTDOWN;

	for (stdext::hash_map<std::wstring, MasterPlayerContext*>::iterator mit = m_mPlayerContextByName.begin();
		mit != m_mPlayerContextByName.end(); ++mit)
	{
		ShutdownPlayer(mit->second);
	}
}

void MasterServerLoop::_OnCommandOnLoginReport(LogicCommandOnLoginReport* pCommand)
{
	m_LoginServerContext.m_bIsConnected = true;
	m_LoginServerContext.m_iServerId = pCommand->m_iServerId;
	m_LoginServerContext.m_dwLastReportTime = GetCurrTime();
}

void MasterServerLoop::_OnCommandOnGateReport(LogicCommandOnGateReport* pCommand)
{
	GateServerContext* pContext = NULL;

	if (pCommand->m_iServerId > SERVERCOUNT_MAX ||
		!m_arrayGateServerContext[pCommand->m_iServerId])
	{
		LOG_ERR(LOG_SERVER, _T("invalid server id=%d"), pCommand->m_iServerId);
		_ASSERT(false && _T("invalid server id"));
		return;
	}

	pContext = m_arrayGateServerContext[pCommand->m_iServerId];
	pContext->m_bIsConnected = true;
	pContext->m_dwLastReportTime = GetCurrTime();
	pContext->m_iSessionCount = pCommand->m_iSessionCount;
}

void MasterServerLoop::_OnCommandOnLoginReq(LogicCommandOnLoginReq* pCommand)
{
	MasterPlayerContext* pPlayerContext = NULL;
	int32 iRet = 0;

	// check if login server is connected
	if (!m_LoginServerContext.m_bIsConnected)
	{
		LOG_ERR(LOG_SERVER, _T("login server is not connected"));
		iRet = LoginPeerSend::OnLoginFailedAck(g_pServer->m_pLoginServer, pCommand->m_iSessionId, 1);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x OnLoginFailedAck failed"), pCommand->m_strAccountName, pCommand->m_iSessionId);
		}

		return;
	}

	stdext::hash_map<std::wstring, MasterPlayerContext*>::iterator mit = m_mPlayerContextByName.find(pCommand->m_strAccountName);
	if (mit != m_mPlayerContextByName.end())
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x This account is already logged in"), pCommand->m_strAccountName, pCommand->m_iSessionId);
		pPlayerContext = mit->second;

		iRet = LoginPeerSend::OnLoginFailedAck(g_pServer->m_pLoginServer, pCommand->m_iSessionId, 2);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x OnLoginFailedAck failed"), pCommand->m_strAccountName, pCommand->m_iSessionId);
		}

		ShutdownPlayer(pPlayerContext);
		return;
	}
	
	pPlayerContext = m_PlayerContextPool.Allocate();
	if (!pPlayerContext)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x Allocate player context from pool failed"), pCommand->m_strAccountName, pCommand->m_iSessionId);
		iRet = LoginPeerSend::OnLoginFailedAck(g_pServer->m_pLoginServer, pCommand->m_iSessionId, 2);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x OnLoginFailedAck failed"), pCommand->m_strAccountName, pCommand->m_iSessionId);
		}
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x Allocate player context success"), pCommand->m_strAccountName, pCommand->m_iSessionId);

	m_mPlayerContextByName.insert(std::make_pair(pCommand->m_strAccountName, pPlayerContext));
	m_mPlayerContextBySessionId.insert(std::make_pair(pCommand->m_iSessionId, pPlayerContext));
	m_LoginServerContext.m_mPlayerContext.insert(std::make_pair(pCommand->m_iSessionId, pPlayerContext));

	pPlayerContext->OnLoginReq(pCommand->m_iSessionId, pCommand->m_strAccountName);
}

void MasterServerLoop::_OnCommandGateAllocAck(LogicCommandGateAllocAck* pCommand)
{
	MasterPlayerContext* pPlayerContext = NULL;
	stdext::hash_map<std::wstring, MasterPlayerContext*>::iterator mit = m_mPlayerContextByName.find(pCommand->m_strAccountName);
	if (mit != m_mPlayerContextByName.end())
	{
		pPlayerContext = mit->second;
		if (pPlayerContext->m_iSessionId == pCommand->m_iLoginSessionId)
		{
			pPlayerContext->GateAllocAck(pCommand->m_iServerId, pCommand->m_iGateSessionId);
		}
		else
		{
			LOG_ERR(LOG_SERVER, _T("sessionId is different, sessionId1=%d, sessionId2=%d"), pPlayerContext->m_iSessionId, pCommand->m_iLoginSessionId);
		}
	}
	else
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s does not exist in master server"), pCommand->m_strAccountName);
	}
}

void MasterServerLoop::_OnCommandOnGateLoginReq(LogicCommandOnGateLoginReq* pCommand)
{
	stdext::hash_map<uint32, MasterPlayerContext*>::iterator mit = m_mPlayerContextBySessionId.find(pCommand->m_iSessionId);
	if (mit == m_mPlayerContextBySessionId.end())
	{
		LOG_WAR(LOG_SERVER, _T("acc=? sid=%08x can't find context"), pCommand->m_iSessionId);
		return;
	}

	mit->second->OnGateLoginReq();
}

void MasterServerLoop::_OnCommandOnRegionAllocAck(LogicCommandOnRegionAllocAck* pCommand)
{
	stdext::hash_map<uint32, MasterPlayerContext*>::iterator mit = m_mPlayerContextBySessionId.find(pCommand->m_iSessionId);
	if (mit == m_mPlayerContextBySessionId.end())
	{
		LOG_WAR(LOG_SERVER, _T("acc=? sid=%08x can't find context"), pCommand->m_iSessionId);
		return;
	}

	mit->second->OnRegionAllocAck(pCommand->m_iServerId, pCommand->m_iReturn);
}

void MasterServerLoop::_OnCommandOnRegionLeaveReq(LogicCommandOnRegionLeaveReq* pCommand)
{
	stdext::hash_map<uint32, MasterPlayerContext*>::iterator mit = m_mPlayerContextBySessionId.find(pCommand->m_iSessionId);
	if (mit == m_mPlayerContextBySessionId.end())
	{
		LOG_WAR(LOG_SERVER, _T("acc=? sid=%08x can't find context"), pCommand->m_iSessionId);
		return;
	}

	mit->second->OnRegionLeaveReq(pCommand->m_iRegionServerId);
}

void MasterServerLoop::_OnCommandOnSessionDisconnect(LogicCommandOnSessionDisconnect* pCommand)
{
	stdext::hash_map<uint32, MasterPlayerContext*>::iterator mit = m_mPlayerContextBySessionId.find(pCommand->m_iSessionId);
	if (mit != m_mPlayerContextBySessionId.end())
	{
		LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x receive disconnect from session server"), mit->second->m_strAccountName, pCommand->m_iSessionId);
		mit->second->OnSessionDisconnect();
	}
}

void MasterServerLoop::_OnCommandPacketForward(LogicCommandPacketForward* pCommand)
{
	stdext::hash_map<uint32, MasterPlayerContext*>::iterator mit = m_mPlayerContextBySessionId.find(pCommand->m_iSessionId);
	if (mit != m_mPlayerContextBySessionId.end())
	{
		mit->second->DelaySendData(pCommand->m_iTypeId, pCommand->m_iLen, pCommand->m_pData);
		if (!Receiver::OnPacketReceived((void*)mit->second, pCommand->m_iTypeId, pCommand->m_iLen, pCommand->m_pData))
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x on packet received failed"), mit->second->m_strAccountName, pCommand->m_iSessionId);
		}
	}
}