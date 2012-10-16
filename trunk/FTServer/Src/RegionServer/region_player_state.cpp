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

	// when state is alloc ack
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_REGIONALLOCACK);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_ONREGIONENTERREQ, PLAYER_STATE_ONREGIONENTERREQ);

	// when state is receive region enter req
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_ONREGIONENTERREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_REGIONENTERREQ, PLAYER_STATE_REGIONENTERREQ);

	// when state is send region enter req
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_REGIONENTERREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_ONREGIONENTERACK, PLAYER_STATE_ONREGIONENTERACK);

	// when state is receive region enter ack
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_ONREGIONENTERACK);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_SERVERTIMENTF, PLAYER_STATE_SERVERTIMENTF);

	// when state is send server time
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_SERVERTIMENTF);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_ONCLIENTTIMEREQ, PLAYER_STATE_ONCLIENTTIMEREQ);

	// when state is receive client time
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_ONCLIENTTIMEREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_SERVERTIME2NTF, PLAYER_STATE_SERVERTIME2NTF);

	// when state is send server time 2nd
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_SERVERTIME2NTF);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	// todo: finish time synchronization
}