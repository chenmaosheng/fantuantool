#include "master_player_context.h"

void MasterPlayerContext::_InitStateMachine()
{
	FSMState* pState = NULL;

	// when state is none
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_NONE);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_ONLOGINREQ, PLAYER_STATE_ONLOGINREQ);

	// when state is on login req
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_ONLOGINREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_GATEHOLDREQ, PLAYER_STATE_GATEHOLDREQ);
	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_GATEHOLDREQ);

	// when state is gate hold req
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_GATEHOLDREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_GATEHOLDACK, PLAYER_STATE_GATEHOLDACK);
	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_GATEHOLDACK);

	// when state is gate hold ack
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_GATEHOLDACK);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_GATEHOLDNTF, PLAYER_STATE_GATEHOLDNTF);
	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_GATEHOLDACK);

	// when state is gate hold ntf
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_GATEHOLDNTF);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_ONGATELOGINREQ, PLAYER_STATE_ONGATELOGINREQ);
	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_GATEHOLDNTF);

	// when state is on gate login req
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_ONGATELOGINREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_ONGATELOGINREQ);
}