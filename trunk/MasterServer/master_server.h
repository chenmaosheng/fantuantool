/*****************************************************************************************
	filename:	master_server.h
	created:	09/27/2012
	author:		chen
	purpose:	manage all other servers and all player in the entire world

*****************************************************************************************/

#ifndef _H_MASTER_SERVER
#define _H_MASTER_SERVER

#include "server_base.h"
#include "singleton.h"

class MasterServerConfig;
class MasterServer : public ServerBase,
					public Singleton<MasterServer>
{
public:
	typedef ServerBase super;

	// cstr
	MasterServer();

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
	PEER_SERVER m_pLoginServer;	// handle of login peer server
};

extern MasterServer* g_pServer;
extern MasterServerConfig* g_pServerConfig;

#endif
