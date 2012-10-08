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

	pState->AddTransition(PLAYER_EVENT_GATEALLOCREQ, PLAYER_STATE_GATEALLOCREQ);
	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_GATEALLOCREQ);

	// when state is gate allocate req
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_GATEALLOCREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_GATEALLOCACK, PLAYER_STATE_GATEALLOCACK);
	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_GATEALLOCACK);

	// when state is gate allocate ack
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_GATEALLOCACK);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_GATEALLOCNTF, PLAYER_STATE_GATEALLOCNTF);
	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_GATEALLOCACK);

	// when state is gate allocate ntf
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_GATEALLOCNTF);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_ONGATELOGINREQ, PLAYER_STATE_ONGATELOGINREQ);
	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_GATEALLOCNTF);

	// when state is on gate login req
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_ONGATELOGINREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_ONGATELOGINREQ);
	pState->AddTransition(PLAYER_EVENT_CACHELOGINREQ, PLAYER_STATE_CACHELOGINREQ);

	// when state is cache login req
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_CACHELOGINREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_CACHELOGINREQ);
	pState->AddTransition(PLAYER_EVENT_ONAVATARLISTREQ, PLAYER_STATE_ONAVATARLISTREQ);

	// when state is on avatar list req
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_ONAVATARLISTREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_AVATARLISTREQ, PLAYER_STATE_AVATARLISTREQ);
	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_ONAVATARLISTREQ);

	// when state is avatar list req
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_AVATARLISTREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_AVATARLISTREQ);
}