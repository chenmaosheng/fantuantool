#ifndef _H_MASTER_SERVER
#define _H_MASTER_SERVER

#include "server_base.h"
#include "singleton.h"

class MasterServer : public ServerBase,
					public Singleton<MasterServer>
{
public:
	typedef ServerBase super;

	MasterServer();

	int32 Init();
	void Destroy();
	void Shutdown();

private:
	void InitPacketDispatch();

	int32 InitMainLoop();
	void DestroyMainLoop();
};

extern MasterServer* g_pServer;

#endif
