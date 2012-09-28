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

	MasterServer();

	int32 Init(const TCHAR* strServerName);
	void Destroy();
	void Shutdown();

private:
	int32 InitMainLoop();
	void DestroyMainLoop();

	ServerConfig* CreateConfig(uint32 iRealmId, const TCHAR* strServerName);

public:
	PEER_SERVER m_pLoginServer;
};

extern MasterServer* g_pServer;
extern MasterServerConfig* g_pServerConfig;

#endif
