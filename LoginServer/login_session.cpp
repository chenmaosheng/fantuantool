#include "login_session.h"
#include "login_server_loop.h"
#include "master_peer_send.h"
#include "login_server.h"
#include "packet.h"
#include "version.h"
#include "login_server_send.h"

LoginServerLoop* LoginSession::m_pMainLoop = NULL;

LoginSession::LoginSession()
{
	Clear();

	// initialize state machine
	InitStateMachine();
}

LoginSession::~LoginSession()
{
	Clear();
}

void LoginSession::Clear()
{
	super::Clear();
	m_strAccountName[0] = '\0';
}

int32 LoginSession::OnConnection(ConnID connId)
{
	int32 iRet = 0;

	// check state
	if (m_StateMachine.StateTransition(SESSION_EVENT_ONCONNECTION, false) != SESSION_STATE_ONCONNECTION)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%d state=%d Session state error"), m_iSessionId, m_StateMachine.GetCurrState());
		return -1;
	}

	iRet = super::OnConnection(connId);
	if (iRet != 0)
	{
		return iRet;
	}

	LOG_DBG(LOG_SERVER, _T("sid=%d OnConnection success"), m_iSessionId);
	return 0;
}

void LoginSession::OnDisconnect()
{
	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%d Receive disconnect request from client"), m_strAccountName, m_iSessionId);
	
	// check state
	if (m_StateMachine.StateTransition(SESSION_EVENT_ONDISCONNECT, false) != SESSION_STATE_ONDISCONNECT) 
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%d state=%d Session state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	// check if state is login req, it must notify master server
	// and the server is not ready for shutdown
	if (m_StateMachine.GetCurrState() == SESSION_STATE_ONLOGINREQ &&
		!g_pServer->m_bReadyForShutdown)
	{
		// todo: notify master server
	}

	super::OnDisconnect();
}

void LoginSession::Disconnect()
{
	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%d force disconnect client"), m_strAccountName, m_iSessionId);
	super::Disconnect();
}

void LoginSession::OnLoginAck(int32 iReturn)
{
	// this message means login failed
	int32 iRet = 0;

	// check state
	if (m_StateMachine.StateTransition(SESSION_EVENT_ONLOGINACK) != SESSION_STATE_ONLOGINACK)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%d state=%d Session state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		Disconnect();
		return;
	}

	//iRet =
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%d login ack to client failed"), m_strAccountName, m_iSessionId);
		Disconnect();
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%d ret=%d send login ack success"), m_strAccountName, m_iSessionId, iReturn);
	Disconnect();
}

void LoginSession::OnVersionReq(uint32 iVersion)
{
	int32 iRet = 0;

	// check state
	if (m_StateMachine.StateTransition(SESSION_EVENT_ONVERSIONREQ) != SESSION_STATE_ONVERSIONREQ)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%d state=%d Session state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		Disconnect();
		return;
	}

	// check version
	if (iVersion != CLIENT_VERSION)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%d version is invalid, version=%d latest_version=%d"), m_strAccountName, m_iSessionId, iVersion, CLIENT_VERSION);
		
		iRet = LoginServerSend::VersionAck(this, 1);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("acc=%s sid=%d VersionAck failed"), m_strAccountName, m_iSessionId);
		}

		Disconnect();
		return;
	}

	iRet = LoginServerSend::VersionAck(this, 0);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%d VersionAck failed"), m_strAccountName, m_iSessionId);
		Disconnect();
		return;
	}

	iRet = MasterPeerSend::OnLoginReq(g_pServer->m_pMasterServer, m_iSessionId, (uint16)wcslen(m_strAccountName)+1, m_strAccountName);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%d LoginReq to master server failed"), m_strAccountName, m_iSessionId);
		Disconnect();
		return;
	}

	if (m_StateMachine.StateTransition(SESSION_EVENT_ONLOGINREQ, false) != SESSION_STATE_ONLOGINREQ)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%d state=%d Session state error"), m_strAccountName, m_iSessionId, m_StateMachine.GetCurrState());
		return;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%d LoginReq to master server success"), m_strAccountName, m_iSessionId);
}

void LoginSession::InitStateMachine()
{
	FSMState* pState = NULL;
	
	super::InitStateMachine();

	// when state is onconnection
	pState = m_StateMachine.ForceGetFSMState(SESSION_STATE_ONCONNECTION);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(SESSION_EVENT_LOGGEDIN, SESSION_STATE_LOGGEDIN);

	// when state is loggedin
	pState = m_StateMachine.ForceGetFSMState(SESSION_STATE_LOGGEDIN);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(SESSION_EVENT_DISCONNECT, SESSION_STATE_DISCONNECT);
	pState->AddTransition(SESSION_EVENT_ONDISCONNECT, SESSION_STATE_ONDISCONNECT);
	pState->AddTransition(SESSION_EVENT_ONDATA, SESSION_STATE_LOGGEDIN);
	pState->AddTransition(SESSION_EVENT_SEND, SESSION_STATE_LOGGEDIN);
	pState->AddTransition(SESSION_EVENT_ONVERSIONREQ, SESSION_STATE_ONVERSIONREQ);

	// when state is version req
	pState = m_StateMachine.ForceGetFSMState(SESSION_STATE_ONVERSIONREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(SESSION_EVENT_DISCONNECT, SESSION_STATE_DISCONNECT);
	pState->AddTransition(SESSION_EVENT_ONDISCONNECT, SESSION_STATE_ONDISCONNECT);
	pState->AddTransition(SESSION_EVENT_ONLOGINREQ, SESSION_STATE_ONLOGINREQ);
	pState->AddTransition(SESSION_EVENT_SEND, SESSION_STATE_ONVERSIONREQ);

	// when state is login req
	pState = m_StateMachine.ForceGetFSMState(SESSION_STATE_ONLOGINREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(SESSION_EVENT_DISCONNECT, SESSION_STATE_DISCONNECT);
	pState->AddTransition(SESSION_EVENT_ONDISCONNECT, SESSION_STATE_ONDISCONNECT);
	pState->AddTransition(SESSION_EVENT_ONDATA, SESSION_STATE_ONLOGINREQ);
	pState->AddTransition(SESSION_EVENT_SEND, SESSION_STATE_ONLOGINREQ);
	pState->AddTransition(SESSION_EVENT_ONLOGINACK, SESSION_STATE_ONLOGINACK);
	pState->AddTransition(SESSION_EVENT_ONGATEHOLDNTF, SESSION_STATE_ONGATEHOLDNTF);

	// when state is on login ack
	pState = m_StateMachine.ForceGetFSMState(SESSION_STATE_ONLOGINACK);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(SESSION_EVENT_DISCONNECT, SESSION_STATE_DISCONNECT);
	pState->AddTransition(SESSION_EVENT_ONDISCONNECT, SESSION_STATE_ONDISCONNECT);
	pState->AddTransition(SESSION_EVENT_ONDATA, SESSION_STATE_ONLOGINACK);
	pState->AddTransition(SESSION_EVENT_SEND, SESSION_STATE_ONLOGINACK);
	
	// when state is gate hold ntf
	pState = m_StateMachine.ForceGetFSMState(SESSION_STATE_ONGATEHOLDNTF);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(SESSION_EVENT_DISCONNECT, SESSION_STATE_DISCONNECT);
	pState->AddTransition(SESSION_EVENT_ONDISCONNECT, SESSION_STATE_ONDISCONNECT);
	pState->AddTransition(SESSION_EVENT_ONDATA, SESSION_STATE_ONGATEHOLDNTF);
	pState->AddTransition(SESSION_EVENT_SEND, SESSION_STATE_ONGATEHOLDNTF);
}

int32 LoginSession::CheckLoginToken(uint16 iLen, char* pBuf)
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

	iRet = Char2WChar(strAccountName, m_strAccountName, _countof(m_strAccountName));
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

int32 LoginSession::LoggedInNtf()
{
	int32 iRet = 0;
	iRet = super::LoggedInNtf();
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("acc=%s sid=%08x loggin ntf send failed"), m_strAccountName, m_iSessionId);
		return iRet;
	}

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x loggin ntf send success"), m_strAccountName, m_iSessionId);
	return 0;
}

int32 Sender::SendPacket(void* pClient, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	return ((LoginSession*)pClient)->SendData(iTypeId, iLen, pBuf);
}