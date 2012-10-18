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
	PLAYER_STATE_ONAVATARCREATEREQ, // receive avatar create request
	PLAYER_STATE_AVATARCREATEREQ, // send avatar create request to cache server
	PLAYER_STATE_ONAVATARCREATEACK, // receive new avatar
	PLAYER_STATE_ONAVATARSELECTREQ, // receive avatar select request
	PLAYER_STATE_AVATARSELECTREQ, // send avatar select request to cache server
	PLAYER_STATE_ONAVATARSELECTACK, // receive avatar select ack
	PLAYER_STATE_AVATARSELECTACK, // send avatar select ack
	PLAYER_STATE_CHANNELLISTNTF, // send channel list to client
	PLAYER_STATE_ONCHANNELSELECTREQ, // receive channel select request
	PLAYER_STATE_REGIONALLOCREQ, // send region alloc req to region server
	PLAYER_STATE_ONREGIONALLOCACK, // receive region alloc ack
	PLAYER_STATE_CHANNELSELECTACK, // send channel select ack
	PLAYER_STATE_REGIONENTERREQ, // send region enter req
	PLAYER_STATE_ONCHANNELLEAVEREQ, // receive channel leave request
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
	PLAYER_EVENT_ONAVATARCREATEREQ,
	PLAYER_EVENT_AVATARCREATEREQ,
	PLAYER_EVENT_ONAVATARCREATEACK,
	PLAYER_EVENT_AVATARCREATEACK,
	PLAYER_EVENT_ONAVATARSELECTREQ,
	PLAYER_EVENT_AVATARSELECTREQ,
	PLAYER_EVENT_ONAVATARSELECTACK,
	PLAYER_EVENT_AVATARSELECTACK,
	PLAYER_EVENT_CHANNELLISTNTF,
	PLAYER_EVENT_ONCHANNELSELECTREQ,
	PLAYER_EVENT_REGIONALLOCREQ,
	PLAYER_EVENT_ONREGIONALLOCACK,
	PLAYER_EVENT_CHANNELSELECTACK,
	PLAYER_EVENT_REGIONENTERREQ,
	PLAYER_EVENT_ONCHANNELLEAVEREQ,
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
	void OnAvatarListAck(int32 iReturn, uint8 iAvatarCount, const prdAvatar* pAvatar);
	// receive create avatar request
	void OnAvatarCreateReq(prdAvatarCreateData& data);
	// receive create avatar result
	void OnAvatarCreateAck(int32 iReturn, prdAvatar& newAvatar);
	// receive select avatar request
	void OnAvatarSelectReq(const TCHAR* strAvatarName);
	// receive select avatar result
	void OnAvatarSelectAck(int32 iReturn, prdAvatarSelectData& data);
	// send channel list to client
	int32 SendChannelList(uint8 iChannelCount, ftdChannelData* arrayData);
	// receive select channel request
	void OnChannelSelectReq(const TCHAR* strChannelName);
	// receive leave channel request
	void OnChannelLeaveReq();
	// receive region alloc ack
	void OnRegionAllocAck(uint8 iRegionServerId, int32 iReturn);

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
	TCHAR m_strAvatarName[AVATARNAME_MAX+1];
	uint64 m_iAvatarId;
	uint8 m_iLastChannelId;
	uint8 m_iRegionServerId;

	static MasterServerLoop* m_pMainLoop;
	
private:
	// restore delayed send data, only for single thread condition
	static uint16 m_iDelayTypeId;
	static uint16 m_iDelayLen;
	static char m_DelayBuf[MAX_INPUT_BUFFER];
};

#endif
