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
};

extern MasterServer* g_pServer;
extern MasterServerConfig* g_pConfig;

#endif
