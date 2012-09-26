#ifndef _H_GATE_SERVER
#define _H_GATE_SERVER

#include "session_server.h"
#include "singleton.h"

class GateServerConfig;
class GateServer : public SessionServer,
					public Singleton<GateServer>
{
public:
	typedef SessionServer super;

	GateServer();
	
	int32 Init(const TCHAR* strServerName);
	void Destroy();

private:
	int32 InitMainLoop();
	void DestroyMainLoop();

	ServerConfig* CreateConfig(uint32 iRealmId, const TCHAR* strServerName);
	int32 GetServerAndPeerConfig(uint32& iPeerIP, uint16& iPeerPort, uint32& iServerIP, uint16& iServerPort, uint32& iThreadCount);

public:
	PEER_SERVER m_pMasterServer;
};

extern GateServer* g_pServer;
extern GateServerConfig* g_pConfig;

#endif
