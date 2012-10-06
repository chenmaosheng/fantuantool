/*****************************************************************************************
	filename:	state_machine.h
	created:	09/29/2012
	author:		chen
	purpose:	finite state machine base class

*****************************************************************************************/

#ifndef _H_STATE_MACHINE
#define _H_STATE_MACHINE

#include "common.h"
#include <map>

#define INVALID_FSM_STATE -1

// state machine's state declaration
class FSMState
{
public:
	FSMState(int32 iState);

	// get this state
	int32 GetState() const
	{
		return m_iState;
	}

	// add new event/state pair
	void AddTransition(int32 iEvent, int32 iOutputState);

	// clear a event/state pair
	void DeleteTransition(int32 iEvent);

	// get state by event
	int32 GetOutputState(int32 iEvent);
	
private:
	int32 m_iState;	// fsm's state
	// use event as array's index, state as value
	int32 m_arrayOutputState[FSM_STATE_MAX]; // all possible output states
};


class StateMachine
{
public:
	StateMachine(int32 iInitialState);
	~StateMachine();

	// set current state
	void SetCurrState(int32 iState);

	// get current state
	int32 GetCurrState() const;

	// get fsm state by state id
	FSMState* GetFSMState(int32 iState);

	// add new fsm state
	void AddFSMState(FSMState*);
	
	// check if fsmstate exists, if not, add a new one
	// in any case will return a fsmstate
	FSMState* ForceGetFSMState(int32 iState);

	// delete a fsm state
	void DeleteFSMState(int32 iState);

	// Transition change
	int32 StateTransition(int32 iEvent, bool bNeedChange = true);

private:
	int32 m_iCurrState;
	std::map<int32, FSMState*> m_mStateMap;
};

#endif
