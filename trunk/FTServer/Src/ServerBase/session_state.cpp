#include "session.h"

void Session::InitStateMachine()
{
	FSMState* pState = NULL;

	// when state is none
	pState = m_StateMachine.ForceGetFSMState(SESSION_STATE_NONE);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(SESSION_EVENT_ONCONNECTION, SESSION_STATE_ONCONNECTION);

	// when state is onconnection
	pState = m_StateMachine.ForceGetFSMState(SESSION_STATE_ONCONNECTION);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(SESSION_EVENT_DISCONNECT, SESSION_STATE_DISCONNECT);
	pState->AddTransition(SESSION_EVENT_ONDISCONNECT, SESSION_STATE_ONDISCONNECT);
	pState->AddTransition(SESSION_EVENT_ONDATA, SESSION_STATE_ONCONNECTION);

	// when state is disconnect
	pState = m_StateMachine.ForceGetFSMState(SESSION_STATE_DISCONNECT);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(SESSION_EVENT_ONDISCONNECT, SESSION_STATE_ONDISCONNECT);

	// when state is ondisconnect
	pState = m_StateMachine.ForceGetFSMState(SESSION_STATE_ONDISCONNECT);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}
}
