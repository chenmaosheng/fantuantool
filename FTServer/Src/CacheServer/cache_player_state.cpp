#include "cache_player_context.h"

void CachePlayerContext::_InitStateMachine()
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

	pState->AddTransition(PLAYER_EVENT_AVATARLISTREQ, PLAYER_STATE_AVATARLISTREQ);
	pState->AddTransition(PLAYER_EVENT_ONLOGOUTREQ, PLAYER_STATE_ONLOGOUTREQ);

	// when state is avatar list req
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_AVATARLISTREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_AVATARLISTACK, PLAYER_STATE_AVATARLISTACK);
	
	// when state is avatar list ack
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_AVATARLISTACK);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_ONLOGOUTREQ, PLAYER_STATE_ONLOGOUTREQ);
}