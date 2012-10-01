#include "gate_session.h"
#include "gate_server_loop.h"
#include "gate_server.h"
#include "master_peer_send.h"
#include "gate_server_config.h"
#include "packet.h"

GateServerLoop* GateSession::m_pMainLoop = NULL;

GateSession::GateSession()
{
	Clear();

	// initialize state machine
	InitStateMachine();
}

GateSession::~GateSession()
{
	Clear();
}

void GateSession::Clear()
{
	super::Clear();
}

int32 GateSession::OnConnection(ConnID connId)
{
	LOG_DBG(LOG_SERVER, _T("Receive a connection, connId=%d"), connId);
	return super::OnConnection(connId);
}

void GateSession::OnDisconnect()
{
	LOG_DBG(LOG_SERVER, _T("sid=%08x connection is disconnected"), m_iSessionId);
	super::OnDisconnect();
}

void GateSession::Disconnect()
{
	LOG_DBG(LOG_SERVER, _T("sid=%08x force to disconnect"), m_iSessionId);
	super::Disconnect();
}

void GateSession::OnHoldReq(uint32 iLoginSessionId, const TCHAR *strAccountName)
{
	int32 iRet = 0;

	((SessionId*)&m_iSessionId)->sValue_.sequence_++;
	wcscpy_s(m_strAccountName, sizeof(m_strAccountName)/sizeof(TCHAR), strAccountName);

	iRet = MasterPeerSend::GateHoldAck(g_pServer->m_pMasterServer, g_pServerConfig->m_iServerId, iLoginSessionId, strAccountName, m_iSessionId);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x gate hold ack failed to send"), m_iSessionId);
		return;
	}

	LOG_DBG(LOG_SERVER, _T("sid=%08x send gate hold ack to master server"), m_iSessionId);
}

void GateSession::InitStateMachine()
{
	FSMState* pState = NULL;
	
	super::InitStateMachine();

	// when state is none
	pState = m_StateMachine.ForceGetFSMState(SESSION_STATE_NONE);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(SESSION_EVENT_ONGATEHOLDREQ, SESSION_STATE_ONGATEHOLDREQ);
	
	// when state is connected
	pState = m_StateMachine.ForceGetFSMState(SESSION_STATE_ONCONNECTION);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}
	pState->AddTransition(SESSION_EVENT_LOGGEDIN, SESSION_STATE_TRANSFERED);

	// when state is transfered
	pState = m_StateMachine.ForceGetFSMState(SESSION_STATE_TRANSFERED);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	// when state is receive gate hold req
	pState = m_StateMachine.ForceGetFSMState(SESSION_STATE_ONGATEHOLDREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(SESSION_EVENT_GATEHOLDACK, SESSION_STATE_GATEHOLDACK);

	// when state is send gate hold ack
	pState = m_StateMachine.ForceGetFSMState(SESSION_STATE_GATEHOLDACK);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(SESSION_EVENT_TRANSFERED, SESSION_STATE_LOGGEDIN);

	// when state is gate release req
	pState = m_StateMachine.ForceGetFSMState(SESSION_STATE_GATERELEASEREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	// when state is loggedin
	pState = m_StateMachine.ForceGetFSMState(SESSION_STATE_LOGGEDIN);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(SESSION_EVENT_GATELOGINREQ, SESSION_STATE_GATELOGINREQ);

	// when state is send gate login
	pState = m_StateMachine.ForceGetFSMState(SESSION_STATE_GATELOGINREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(SESSION_EVENT_DISCONNECT, SESSION_STATE_DISCONNECT);
	pState->AddTransition(SESSION_EVENT_ONDISCONNECT, SESSION_STATE_ONDISCONNECT);
	pState->AddTransition(SESSION_EVENT_ONDATA, SESSION_STATE_GATELOGINREQ);
	pState->AddTransition(SESSION_EVENT_SEND, SESSION_STATE_GATELOGINREQ);

}

int32 GateSession::CheckLoginToken(uint16 iLen, char* pBuf)
{
	int32 iRet = 0;
	// todo: pBuf's format is (Account;Password)
	char strAccountName[ACCOUNTNAME_MAX*3 + 1] = {0};
	char strPassword[PASSWORD_MAX+1] = {0};
	char* nextToken = NULL;

	// get account and password from token
	char* pToken = strtok_s(pBuf, ";", &nextToken);
	if (!pToken || strlen(pToken) > ACCOUNTNAME_MAX*3 + 1)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x token is invalid"), m_iSessionId);
		return -1;
	}

	strcpy_s(strAccountName, sizeof(strAccountName), pToken);

	pToken = strtok_s(NULL, ";", &nextToken);
	if (!pToken || strlen(pToken) > PASSWORD_MAX + 1)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x token is invalid"), m_iSessionId);
		return -1;
	}

	strcpy_s(strPassword, sizeof(strPassword), pToken);

	iRet = Char2WChar(strAccountName, m_strAccountName, sizeof(m_strAccountName)/sizeof(TCHAR));
	if (iRet == 0)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x Char2WChar error"), m_iSessionId);
		return -1;
	}

	// check if accountName is empty
	if (wcslen(m_strAccountName) == 0)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x m_strAccountName is empty"), m_iSessionId);
		return -1;
	}

	// change accountName to lowercase
	LowerCase(m_strAccountName);

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x Token is valid"), m_strAccountName, m_iSessionId);

	return 0;
}

int32 GateSession::LoggedInNtf()
{
	int32 iRet = 0;
	GateSession* pSession = NULL;

	// check state
	if (m_StateMachine.StateTransition(SESSION_EVENT_LOGGEDIN, false) != -1)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%d state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return -1;
	}

	iRet = m_pMainLoop->TransferSession(m_iSessionId, m_strAccountName, pSession);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%d TransferSession error"), m_strAccountName, m_iSessionId);
		Disconnect();
		return -1;
	}

	// after transfer session, check state again
	if (m_StateMachine.StateTransition(SESSION_EVENT_LOGGEDIN, false) != SESSION_STATE_TRANSFERED)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%d state=%d state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return -1;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%d TempSession finishes itse task"), m_strAccountName, m_iSessionId);

	m_pMainLoop->CloseSession(this);

	return 0;
}

int32 Sender::SendPacket(void* pClient, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	return ((GateSession*)pClient)->SendData(iTypeId, iLen, pBuf);
}
