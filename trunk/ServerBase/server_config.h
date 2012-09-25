#ifndef _H_SERVER_CONFIG
#define _H_SERVER_CONFIG

#include "server_common.h"

struct ServerConfig
{
	ServerConfig(uint32 iRealmId);
	virtual ~ServerConfig();

	TCHAR m_strServerName[MAX_PATH+1];
	uint16 m_iServerId;
	uint32 m_iPeerIP;
	uint16 m_iPeerPort;
	uint32 m_iRealmId;

};

#endif
