#ifndef _H_MASTER_PLAYER_CONTEXT
#define _H_MASTER_PLAYER_CONTEXT

#include "server_common.h"

class MasterServerLoop;
class MasterPlayerContext
{
public:
	MasterPlayerContext();
	~MasterPlayerContext();

	void Clear();

	void OnLoginReq(uint32 iSessionId, const TCHAR* strAccountName);
	
public:
	uint32 m_iSessionId;
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
	static MasterServerLoop* m_pMainLoop;

private:
	uint16 m_iGateServerId;
};

#endif
