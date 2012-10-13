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

	pState->AddTransition(PLAYER_EVENT_ONAVATARLISTACK, PLAYER_STATE_ONAVATARLISTACK);
	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_AVATARLISTREQ);

	// when state is on avatar list ack
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_ONAVATARLISTACK);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_AVATARLISTACK, PLAYER_STATE_AVATARLISTACK);
	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_ONAVATARLISTACK);

	// when state is send avatar list ack
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_AVATARLISTACK);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_ONAVATARCREATEREQ, PLAYER_STATE_ONAVATARCREATEREQ);
	pState->AddTransition(PLAYER_EVENT_ONAVATARSELECTREQ, PLAYER_STATE_ONAVATARSELECTREQ);
	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_AVATARLISTACK);

	// when state is on avatar create req
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_ONAVATARCREATEREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_AVATARCREATEREQ, PLAYER_STATE_AVATARCREATEREQ);
	pState->AddTransition(PLAYER_EVENT_AVATARCREATEACK, PLAYER_STATE_AVATARLISTACK);
	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_ONAVATARCREATEREQ);

	// when state is send avatar create req
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_AVATARCREATEREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_ONAVATARCREATEACK, PLAYER_STATE_ONAVATARCREATEACK);
	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_AVATARCREATEREQ);

	// when state is receive new avatar ack
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_ONAVATARCREATEACK);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_AVATARCREATEACK, PLAYER_STATE_AVATARLISTACK);
	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_ONAVATARCREATEACK);

	// when state is receive select avatar req
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_ONAVATARSELECTREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_AVATARSELECTREQ, PLAYER_STATE_AVATARSELECTREQ);
	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_AVATARSELECTREQ);

	// when state is send select avatar req
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_AVATARSELECTREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_ONAVATARSELECTACK, PLAYER_STATE_ONAVATARSELECTACK);
	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_AVATARSELECTREQ);

	// when state is receive select avatar ack
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_ONAVATARSELECTACK);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_AVATARSELECTACK, PLAYER_STATE_AVATARSELECTACK);
	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_ONAVATARSELECTACK);

	// when state is send select avatar ack
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_AVATARSELECTACK);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_CHANNELLISTNTF, PLAYER_STATE_CHANNELLISTNTF);
	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_AVATARSELECTACK);

	// when state is send channel list
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_CHANNELLISTNTF);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_ONCHANNELSELECTREQ, PLAYER_STATE_ONCHANNELSELECTREQ);
	pState->AddTransition(PLAYER_EVENT_ONAVATARLISTREQ, PLAYER_STATE_ONAVATARLISTREQ);
	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_CHANNELLISTNTF);

	// when state is receive channel select
	pState = m_StateMachine.ForceGetFSMState(PLAYER_STATE_ONCHANNELSELECTREQ);
	if (!pState)
	{
		LOG_ERR(LOG_SERVER, _T("Can't get fsm state"));
		return;
	}

	pState->AddTransition(PLAYER_EVENT_ONSESSIONDISCONNECT, PLAYER_STATE_ONCHANNELSELECTREQ);
}
