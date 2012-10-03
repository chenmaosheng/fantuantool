#include "login_session.h"

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