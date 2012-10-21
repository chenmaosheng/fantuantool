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

	pState->AddTransition(PLAYER_EVENT_ONAVATARLISTREQ, PLAYER_STATE_ONAVATARLISTREQ);
	pState->AddTransition(PLAYER_EVENT_ONLOGOUTREQ, PLAYER_STATE_ONLOGOUTREQ);

	// when state is receive avatar list req
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_ONAVATARLISTREQ);
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

	pState->AddTransition(PLAYER_EVENT_ONAVATARSELECTREQ, PLAYER_STATE_ONAVATARSELECTREQ);
	pState->AddTransition(PLAYER_EVENT_ONLOGOUTREQ, PLAYER_STATE_ONLOGOUTREQ);

	// when state is receive avatar select req
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_ONAVATARSELECTREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_AVATARSELECTACK, PLAYER_STATE_AVATARSELECTACK);

	// when state is avatar select ack
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_AVATARSELECTACK);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_ONAVATARLISTREQ, PLAYER_STATE_ONAVATARLISTREQ);
	pState->AddTransition(PLAYER_EVENT_ONREGIONENTERREQ, PLAYER_STATE_ONREGIONENTERREQ);
	pState->AddTransition(PLAYER_EVENT_ONLOGOUTREQ, PLAYER_STATE_ONLOGOUTREQ);

	// when state is receive region enter req
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_ONREGIONENTERREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_REGIONENTERACK, PLAYER_STATE_REGIONENTERACK);
	pState->AddTransition(PLAYER_EVENT_ONLOGOUTREQ, PLAYER_STATE_ONLOGOUTREQ);

	// when state is send region enter ack
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_REGIONENTERACK);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_ONLOGOUTREQ, PLAYER_STATE_ONLOGOUTREQ);
}