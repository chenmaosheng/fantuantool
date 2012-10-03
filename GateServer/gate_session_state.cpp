#include "gate_session.h"

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
