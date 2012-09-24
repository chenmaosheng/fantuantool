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
	
public:
	uint32 m_iSessionId;
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];

public:
	static MasterServerLoop* m_pMainLoop;
};

#endif
