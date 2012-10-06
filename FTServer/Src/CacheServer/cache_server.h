/*****************************************************************************************
	filename:	cache_server.h
	created:	10/05/2012
	author:		chen
	purpose:	cache all data and contact to database

*****************************************************************************************/

#ifndef _H_CACHE_SERVER
#define _H_CACHE_SERVER

#include "server_base.h"
#include "singleton.h"

class CacheServerConfig;
class CacheServer : public ServerBase,
					public Singleton<CacheServer>
{
public:
	typedef ServerBase super;

	// cstr
	CacheServer();

	// initialize Cache server
	int32 Init(const TCHAR* strServerName);
	// destroy Cache server
	void Destroy();
	// shutdown Cache server
	void Shutdown();

private:
	// initialize Cache server's loop
	int32 InitMainLoop();
	// destroy main loop
	void DestroyMainLoop();

	// allocate server configuration
	ServerConfig* CreateConfig(uint32 iRealmId, const TCHAR* strServerName);

public:
	PEER_SERVER m_pMasterServer;	// handle of master peer server
};

extern CacheServer* g_pServer;
extern CacheServerConfig* g_pServerConfig;

#endif
