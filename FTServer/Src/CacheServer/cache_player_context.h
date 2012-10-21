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
#include "cache_avatar_context.h"

enum
{
	PLAYER_STATE_NONE,
	PLAYER_STATE_ONLOGINREQ,	// receive login req
	PLAYER_STATE_ONAVATARLISTREQ, // receive avatar list req
	PLAYER_STATE_AVATARLISTACK, // send avatar list
	PLAYER_STATE_ONAVATARSELECTREQ, // receive avatar select req
	PLAYER_STATE_AVATARSELECTACK, // send avatar select ack
	PLAYER_STATE_ONREGIONENTERREQ, // receive region enter req
	PLAYER_STATE_REGIONENTERACK, // send region enter ack
	PLAYER_STATE_ONLOGOUTREQ,
};

enum
{
	PLAYER_EVENT_ONLOGINREQ,
	PLAYER_EVENT_ONAVATARLISTREQ,
	PLAYER_EVENT_AVATARLISTACK,
	PLAYER_EVENT_ONAVATARSELECTREQ,
	PLAYER_EVENT_AVATARSELECTACK,
	PLAYER_EVENT_ONREGIONENTERREQ,
	PLAYER_EVENT_REGIONENTERACK,
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
	// receive avatar create req
	void OnAvatarCreateReq(prdAvatarCreateData& data);
	// receive avatar select req
	void OnAvatarSelectReq(const TCHAR* strAvatarName);
	// receive save data req, sendntf: if need to send to region
	void OnSaveDataReq(bool bSendNtf=false);
	// receive avatar logout req
	void OnAvatarLogoutReq();

public:	// receive packet handler
	void OnLoginReq(uint32 iSessionId, TCHAR* strAccountName);
	void OnLogoutReq();
	void OnRegionEnterReq(uint8 iServerId, TCHAR* strAvatarName);

public: // receive db event result
	void OnPlayerEventGetAvatarListResult(PlayerDBEventGetAvatarList*);
	void OnPlayerEventAvatarCreateResult(PlayerDBEventAvatarCreate*);
	void OnPlayerEventAvatarSelectResult(PlayerDBEventAvatarSelectData*);
	void OnPlayerEventAvatarEnterRegionResult(PlayerDBEventAvatarEnterRegion*);
	void OnPlayerEventAvatarFinalizeResult(PlayerDBEventAvatarFinalize*);
	void OnPlayerEventAvatarSaveDataResult(PlayerDBEventAvatarSaveData*);
	void OnPlayerEventAvatarLogoutResult(PlayerDBEventAvatarLogout*);

private:
	// initialize state machine
	void _InitStateMachine();

public:
	uint32 m_iSessionId;
	uint8 m_iRegionServerId;
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
	StateMachine m_StateMachine;
	bool m_bFinalizing; // almost leave or not
	uint32 m_iNextSaveDataTime;

	// avatar info list
	uint8 m_iAvatarCount;
	prdAvatar m_arrayAvatar[AVATARCOUNT_MAX];
	CacheAvatarContext m_AvatarContext; // selected avatar context

	static CacheServerLoop* m_pMainLoop;

private:
	// restore delayed send data, only for single thread condition
	static uint16 m_iDelayTypeId;
	static uint16 m_iDelayLen;
	static char m_DelayBuf[MAX_INPUT_BUFFER];
};

#endif ml;