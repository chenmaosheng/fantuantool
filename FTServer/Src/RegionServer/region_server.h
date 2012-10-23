/*****************************************************************************************
	filename:	region_server.h
	created:	10/14/2012
	author:		chen
	purpose:	manager all players in a region

*****************************************************************************************/

#ifndef _H_REGION_SERVER
#define _H_REGION_SERVER

#include "server_base.h"
#include "singleton.h"

class RegionServerConfig;
class DataCenter;
class RegionServer : public ServerBase,
					public Singleton<RegionServer>
{
public:
	typedef ServerBase super;

	// cstr
	RegionServer();

	// initialize master server
	int32 Init(const TCHAR* strServerName);
	// destroy master server
	void Destroy();
	// shutdown master server
	void Shutdown();

private:
	// intialize master server's loop
	int32 InitMainLoop();
	// destroy main loop
	void DestroyMainLoop();

	// allocate server configuration
	ServerConfig* CreateConfig(uint32 iRealmId, const TCHAR* strServerName);

public:
	PEER_SERVER m_pMasterServer;	// handle of master peer server
	PEER_SERVER m_pCacheServer; // handle of cache peer server
};

extern RegionServer* g_pServer;
extern RegionServerConfig* g_pServerConfig;
extern DataCenter* g_pDataCenter;

#endif
