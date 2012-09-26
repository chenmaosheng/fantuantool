#ifndef _H_LOGIN_SERVER
#define _H_LOGIN_SERVER

#include "session_server.h"
#include "singleton.h"

class LoginServerConfig;
class LoginServer : public SessionServer,
					public Singleton<LoginServer>
{
public:
	typedef SessionServer super;

	LoginServer();
	
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

extern LoginServer* g_pServer;
extern LoginServerConfig* g_pConfig;

#endif
