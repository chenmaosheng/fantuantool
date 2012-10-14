/*****************************************************************************************
	filename:	region_player_context.h
	created:	10/14/2012
	author:		chen
	purpose:	record player context on region server

*****************************************************************************************/

#ifndef _H_REGION_PLAYER_CONTEXT
#define _H_REGION_PLAYER_CONTEXT

#include "server_common.h"
#include "state_machine.h"

enum
{
	PLAYER_STATE_NONE,
	PLAYER_STATE_ONALLOCREQ, // receive alloc req
};

enum
{
	PLAYER_EVENT_ONALLOCREQ,
};

class RegionServerLoop;
class RegionPlayerContext
{
public:
	// cstr and dstr
	RegionPlayerContext();
	~RegionPlayerContext();

	// clear all variables
	void Clear();

	void OnRegionAllocReq(uint32 iSessionId, uint64 iAvatarId, const TCHAR* strAvatarName);

public:
	uint32 m_iSessionId;
	uint64 m_iAvatarId;
	TCHAR m_strAvatarName[AVATARNAME_MAX+1];
	bool m_bFinalizing; // almost leave or not
	StateMachine m_StateMachine;
	PEER_SERVER m_pGateServer;	// related gate server

	// map info
	uint32 m_iMapId;

	static RegionServerLoop* m_pMainLoop;
};

#endif 