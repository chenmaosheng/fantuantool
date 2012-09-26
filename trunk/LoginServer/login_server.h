#ifndef _H_LOGIN_SERVER
#define _H_LOGIN_SERVER

#include "session_server.h"
#include "singleton.h"
#include "peer_packet.h"

class LoginServer : public SessionServer,
					public Singleton<LoginServer>
{
public:
	typedef SessionServer super;

	LoginServer();
	
	int32 Init(const TCHAR* strServerName);
	void Destroy();

private:
	void InitPacketDispatch();

	int32 InitMainLoop();
	void DestroyMainLoop();

public:
	PEER_SERVER m_pMasterServer;
};

extern LoginServer* g_pServer;

#endif
