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

enum
{
	AVATAR_STATE_NONE,
	AVATAR_STATE_ONMAPENTERREQ,	// receive map enter
	AVATAR_STATE_ONMAPLEAVEREQ,	// receive map leave
	AVATAR_STATE_MAPLEAVEACK,	// send map leave ack
};

enum
{
	AVATAR_EVENT_ONMAPENTERREQ,
	AVATAR_EVENT_ONMAPLEAVEREQ,
	AVATAR_EVENT_MAPLEAVEACK,
};

class RegionPlayerContext;
class Map;
class Avatar : public Actor
{
public:
	// cstr and dstr
	Avatar();
	~Avatar();

	// enter map
	void OnMapEnterReq(Map* pMap);

	// add avatar to interest
	void AddAvatarToInterestList(Avatar* pAvatar, bool bSendNtf=false);
	// notify map enter to client
	int32 SendAvatarEnterNtf(Avatar*);

private:
	// initialize state machine
	void _InitStateMachine();
	// enter map helper function
	int32 _OnMapEnterReq(Map* pMap);

public:
	RegionPlayerContext* m_pPlayerContext;
	bool m_bFinalizing;
	StateMachine m_StateMachine;

	// avatar info
	TCHAR m_strAvatarName[AVATARNAME_MAX+1];
	uint64 m_iAvatarId;

	// map info
	uint16 m_iTeleportMapId;

	std::list<Actor*> m_AvatarInterestList;
};

#endif
