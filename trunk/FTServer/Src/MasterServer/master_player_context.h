/*****************************************************************************************
	filename:	master_player_context.h
	created:	09/27/2012
	author:		chen
	purpose:	record player context on master server

*****************************************************************************************/

#ifndef _H_MASTER_PLAYER_CONTEXT
#define _H_MASTER_PLAYER_CONTEXT

#include "server_common.h"
#include "state_machine.h"
#include "ftd_define.h"

enum
{
	PLAYER_STATE_NONE,
	PLAYER_STATE_ONLOGINREQ,	// receive login request from login server
	PLAYER_STATE_GATEALLOCREQ,	// send alloc request to gate server
	PLAYER_STATE_GATEALLOCACK,	// receive alloc ack from gate server
	PLAYER_STATE_GATEALLOCNTF,	// send alloc ntf to client
	PLAYER_STATE_ONGATELOGINREQ, // receive login request from gate server
	PLAYER_STATE_CACHELOGINREQ, // send login request to cache server
	PLAYER_STATE_ONAVATARLISTREQ, // receive avatar list request from client
	PLAYER_STATE_AVATARLISTREQ,	// send avatar list request to cache server
	PLAYER_STATE_ONAVATARLISTACK, // receive avatar list
	PLAYER_STATE_AVATARLISTACK, // send avatar list
};

enum
{
	PLAYER_EVENT_ONLOGINREQ,
	PLAYER_EVENT_GATEALLOCREQ,
	PLAYER_EVENT_GATEALLOCACK,
	PLAYER_EVENT_GATEALLOCNTF,
	PLAYER_EVENT_ONGATELOGINREQ,
	PLAYER_EVENT_ONSESSIONDISCONNECT,
	PLAYER_EVENT_CACHELOGINREQ,
	PLAYER_EVENT_ONAVATARLISTREQ,
	PLAYER_EVENT_AVATARLISTREQ,
	PLAYER_EVENT_ONAVATARLISTACK,
	PLAYER_EVENT_AVATARLISTACK,
};

class MasterServerLoop;
class MasterPlayerContext
{
public:
	// cstr and dstr
	MasterPlayerContext();
	~MasterPlayerContext();

	// clear all variables
	void Clear();
	// save need send data to buffer, delay to send by some means, only send to client
	int32 DelaySendData(uint16 iTypeId, uint16 iLen, const char* pBuf);

	// receive login request from login server
	void OnLoginReq(uint32 iSessionId, const TCHAR* strAccountName);
	// receive login request from gate server
	void OnGateLoginReq();
	// send gate allocate response to login server
	void GateAllocAck(uint8 iGateServerId, uint32 iGateSessionId);
	// receive disconnect from session server
	void OnSessionDisconnect();
	// receive avatar list request
	void OnAvatarListReq();
	// receive avatar list result
	void OnAvatarListAck(int32 iRet, uint8 iAvatarCount, const prdAvatar* pAvatar);

private:
	// initialize state machine
	void _InitStateMachine();
	
public:
	uint32 m_iSessionId;
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
	bool m_bFinalizing; // almost leave or not
	StateMachine m_StateMachine;
	uint8 m_iGateServerId;		// gate server's id which this player locates

	// avatar info
	uint8 m_iAvatarCount;
	prdAvatar m_arrayAvatar[AVATARCOUNT_MAX];

	static MasterServerLoop* m_pMainLoop;
	
private:
	// restore delayed send data, only for single thread condition
	static uint16 m_iDelayTypeId;
	static uint16 m_iDelayLen;
	static char m_DelayBuf[MAX_INPUT_BUFFER];
};

#endif
