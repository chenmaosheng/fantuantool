#include "region_player_context.h"

void RegionPlayerContext::_InitStateMachine()
{
	FSMState* pState = NULL;

	// when state is none
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_NONE);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_ONREGIONALLOCREQ, PLAYER_STATE_ONREGIONALLOCREQ);

	// when state is on alloc req
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_ONREGIONALLOCREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_REGIONALLOCACK, PLAYER_STATE_REGIONALLOCACK);
}