/*****************************************************************************************
	filename:	cache_player_context.h
	created:	10/05/2012
	author:		chen
	purpose:	record player context on cache server

*****************************************************************************************/

#ifndef _H_CACHE_PLAYER_CONTEXT
#define _H_CACHE_PLAYER_CONTEXT

#include "server_common.h"
#include "state_machine.h"
#include "cache_db_event.h"

enum
{
	PLAYER_STATE_NONE,
	PLAYER_STATE_ONLOGINREQ,
	PLAYER_STATE_AVATARLISTREQ,
	PLAYER_STATE_AVATARLISTACK,
	PLAYER_STATE_ONLOGOUTREQ,
};

enum
{
	PLAYER_EVENT_ONLOGINREQ,
	PLAYER_EVENT_AVATARLISTREQ,
	PLAYER_EVENT_AVATARLISTACK,
	PLAYER_EVENT_ONLOGOUTREQ,
};

class CacheServerLoop;
class CachePlayerContext
{
public:
	// cstr and dstr
	CachePlayerContext();
	~CachePlayerContext();
	// clear all variables
	void Clear();
	
	// save need send data to buffer, delay to send by some means, only send to client
	int32 DelaySendData(uint16 iTypeId, uint16 iLen, const char* pBuf);
	// shutdown itself
	void Shutdown();
	// receive avatar list req
	void OnAvatarListReq();

public:	// receive packet handler
	void OnLoginReq(uint32 iSessionId, TCHAR* strAccountName);
	void OnLogoutReq();

public: // receive db event result
	void OnPlayerEventGetAvatarListResult(PlayerDBEventGetAvatarList*);

private:
	// initialize state machine
	void _InitStateMachine();

public:
	uint32 m_iSessionId;
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
	StateMachine m_StateMachine;
	bool m_bFinalizing; // almost leave or not

	// avatar info list
	uint8 m_iAvatarCount;
	prdAvatar m_arrayAvatar[AVATARCOUNT_MAX];

	static CacheServerLoop* m_pMainLoop;

private:
	// restore delayed send data, only for single thread condition
	static uint16 m_iDelayTypeId;
	static uint16 m_iDelayLen;
	static char m_DelayBuf[MAX_INPUT_BUFFER];
};

#endif