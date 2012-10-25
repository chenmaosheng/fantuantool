#include "avatar.h"

void Avatar::_InitStateMachine()
{
	FSMState* pState = NULL;

	// when state is none
	pState = m_StateMachine.ForceGetFSMState(AVATAR_STATE_NONE);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(AVATAR_EVENT_ONMAPENTERREQ, AVATAR_STATE_ONMAPENTERREQ);

	// when state is on map enter req
	pState = m_StateMachine.ForceGetFSMState(AVATAR_STATE_ONMAPENTERREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(AVATAR_EVENT_ONMAPLEAVEREQ, AVATAR_STATE_ONMAPLEAVEREQ);

	// when state is on map leave req
	pState = m_StateMachine.ForceGetFSMState(AVATAR_STATE_ONMAPLEAVEREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(AVATAR_EVENT_MAPLEAVEACK, AVATAR_STATE_MAPLEAVEACK);

	// when state is send map leave ack
	pState = m_StateMachine.ForceGetFSMState(AVATAR_STATE_MAPLEAVEACK);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}
}