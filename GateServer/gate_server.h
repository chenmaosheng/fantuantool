#ifndef _H_GATE_SERVER
#define _H_GATE_SERVER

#include "session_server.h"
#include "singleton.h"
#include "peer_packet.h"

class GateServer : public SessionServer,
					public Singleton<GateServer>
{
public:
	typedef SessionServer super;

	GateServer();
	
	int32 Init(const TCHAR* strServerName);
	void Destroy();

private:
	void InitPacketDispatch();

	int32 InitMainLoop();
	void DestroyMainLoop();

public:
	PEER_SERVER m_pMasterServer;
};

extern GateServer* g_pServer;

#endif
