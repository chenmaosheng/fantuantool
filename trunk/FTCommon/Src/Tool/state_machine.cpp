#include "state_machine.h"

FSMState::FSMState(int32 iState) : 
m_iState(iState)
{
	for (int32 i = 0; i < FSM_STATE_MAX; ++i)
	{
		m_arrayOutputState[i] = INVALID_FSM_STATE;
	}
}

void FSMState::AddTransition(int32 iEvent, int32 iOutputState)
{
	if (iEvent >= FSM_STATE_MAX || m_arrayOutputState[iEvent] != INVALID_FSM_STATE)
	{
		return;
	}

	m_arrayOutputState[iEvent] = iOutputState;
}

void FSMState::DeleteTransition(int32 iEvent)
{
	if (iEvent >= FSM_STATE_MAX)
	{
		return;
	}

	m_arrayOutputState[iEvent] = INVALID_FSM_STATE;
}

int32 FSMState::GetOutputState(int32 iEvent)
{
	return m_arrayOutputState[iEvent];
}

StateMachine::StateMachine(int32 iState) :
m_iCurrState(iState)
{
}

StateMachine::~StateMachine()
{
	for (std::map<int32, FSMState*>::iterator mit = m_mStateMap.begin(); mit != m_mStateMap.end(); ++mit)
	{
		SAFE_DELETE(mit->second);
	}
}

void StateMachine::SetCurrState(int32 iState)
{
	m_iCurrState = iState;
}

int32 StateMachine::GetCurrState() const
{
	return m_iCurrState;
}

FSMState* StateMachine::GetFSMState(int32 iState)
{
	std::map<int32, FSMState*>::iterator mit = m_mStateMap.find(iState);
	if (mit != m_mStateMap.end())
	{
		return mit->second;
	}

	return NULL;
}

void StateMachine::AddFSMState(FSMState* pState)
{
	m_mStateMap.insert(std::make_pair(pState->GetState(), pState));
}

FSMState* StateMachine::ForceGetFSMState(int32 iState)
{
	FSMState* pState = GetFSMState(iState);
	if (!pState)
	{
		pState = new FSMState(iState);
		AddFSMState(pState);
	}

	return pState;
}

void StateMachine::DeleteFSMState(int32 iState)
{
	std::map<int32, FSMState*>::iterator mit = m_mStateMap.find(iState);
	if (mit != m_mStateMap.end())
	{
		SAFE_DELETE(mit->second);
		m_mStateMap.erase(mit);
	}
}

int32 StateMachine::StateTransition(int32 iEvent, bool bNeedChange)
{
	int32 iNewState = INVALID_FSM_STATE;
	FSMState* pState = GetFSMState(m_iCurrState);
	if (pState)
	{
		iNewState = pState->GetOutputState(iEvent);
	}

	if (bNeedChange && iNewState != INVALID_FSM_STATE)
	{
		m_iCurrState = iNewState;
	}

	return iNewState;
}