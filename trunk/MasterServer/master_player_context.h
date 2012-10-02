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

enum
{
	PLAYER_STATE_NONE,
	PLAYER_STATE_ONLOGINREQ,
	PLAYER_STATE_GATEHOLDREQ,
	PLAYER_STATE_GATEHOLDACK,
	PLAYER_STATE_GATEHOLDNTF,
	PLAYER_STATE_ONGATELOGINREQ,
};

enum
{
	PLAYER_EVENT_ONLOGINREQ,
	PLAYER_EVENT_GATEHOLDREQ,
	PLAYER_EVENT_GATEHOLDACK,
	PLAYER_EVENT_GATEHOLDNTF,
	PLAYER_EVENT_ONGATELOGINREQ,
	PLAYER_EVENT_ONSESSIONDISCONNECT,
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
	// send gate hold response to login server
	void GateHoldAck(uint16 iGateServerId, uint32 iGateSessionId);
	// receive disconnect from session server
	void OnSessionDisconnect();

private:
	// initialize state machine
	void _InitStateMachine();
	
public:
	uint32 m_iSessionId;
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
	bool m_bFinalizing; // almost leave or not
	StateMachine m_StateMachine;
	uint16 m_iGateServerId;		// gate server's id which this player locates

	static MasterServerLoop* m_pMainLoop;
	
private:
	// restore delayed send data, only for single thread condition
	static uint16 m_iDelayTypeId;
	static uint16 m_iDelayLen;
	static char m_DelayBuf[MAX_INPUT_BUFFER];
};

#endif
