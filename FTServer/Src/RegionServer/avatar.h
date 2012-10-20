/*****************************************************************************************
	filename:	avatar.h
	created:	10/19/2012
	author:		chen
	purpose:	avatar on the map, not include npc

*****************************************************************************************/

#ifndef _H_AVATAR
#define _H_AVATAR

#include "actor.h"
#include "state_machine.h"
#include <list>

class RegionPlayerContext;
class Avatar : public Actor
{
public:
	// cstr and dstr
	Avatar();
	~Avatar();

private:
	// initialize state machine
	void _InitStateMachine();

public:
	RegionPlayerContext* m_pPlayerContext;
	bool m_bFinalizing;
	StateMachine m_StateMachine;

	// avatar info
	TCHAR m_strAvatarName[AVATARNAME_MAX+1];
	uint64 m_iAvatarId;

	std::list<Actor*> m_AvatarInterestList;
};

#endif
