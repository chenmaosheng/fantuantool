/*****************************************************************************************
	filename:	master_player_context.h
	created:	09/27/2012
	author:		chen
	purpose:	record player context on master server

*****************************************************************************************/

#ifndef _H_MASTER_PLAYER_CONTEXT
#define _H_MASTER_PLAYER_CONTEXT

#include "server_common.h"

class MasterServerLoop;
class MasterPlayerContext
{
public:
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
	
public:
	uint32 m_iSessionId;
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
	static MasterServerLoop* m_pMainLoop;

private:
	uint16 m_iGateServerId;		// gate server's id which this player locates

private:
	// restore delayed send data, only for single thread condition
	static uint16 m_iDelayTypeId;
	static uint16 m_iDelayLen;
	static char m_DelayBuf[MAX_INPUT_BUFFER];
};

#endif
