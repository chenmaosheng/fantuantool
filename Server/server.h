#ifndef _H_SERVER
#define _H_SERVER

#include "server_base.h"
#include <string>
#include <vector>

struct Connection;
class Session;
class Server : public ServerBase
{
public:
	Server();
	virtual ~Server();

	int32 Init(const TCHAR* strServerName);
	void Destroy();

	void Start();

	static bool CALLBACK OnConnection(ConnID connId);
	static void CALLBACK OnDisconnect(ConnID connId);
	static void CALLBACK OnData(ConnID connId, uint32 iLen, char* pBuf);
	static void CALLBACK OnConnectFailed(void*);

	static std::string GetNickName(Connection* pConnection);
	static void DeleteClient(Connection* pConnection);

private:
	void InitPacketDispatch();

	int32 InitMainLoop();
	void DestroyMainLoop();

public:
	static std::vector<Connection*> clients;
	static std::vector< std::pair<Connection*, std::string > > nicknames;
};

extern Server* g_pServer;

#endif
