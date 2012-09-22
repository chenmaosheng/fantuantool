#ifndef _H_LOGIN_SERVER
#define _H_LOGIN_SERVER

#include "session_server.h"
#include "singleton.h"

class LoginServer : public SessionServer,
					public Singleton<LoginServer>
{
public:
	typedef SessionServer super;

	LoginServer();
	
	int32 Init();
	void Destroy();

private:
	void InitPacketDispatch();

	int32 InitMainLoop();
	void DestroyMainLoop();
};

extern LoginServer* g_pServer;

#endif
