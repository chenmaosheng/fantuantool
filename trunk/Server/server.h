#ifndef _H_SERVER
#define _H_SERVER

#include "server_base.h"
#include <string>
#include <vector>

struct Connection;
class Server : public ServerBase
{
public:
	Server();
	virtual ~Server();

	int32 Init();
	void Destroy();

	void Start();

	static bool CALLBACK OnConnection(ConnID connID);
	static void CALLBACK OnDisconnect(ConnID connID);
	static void CALLBACK OnData(ConnID connID, uint16 iLen, char* pBuf);
	static void CALLBACK OnConnectFailed(void*);

	static std::string GetNickName(Connection* pConnection);
	static void DeleteClient(Connection* pConnection);

	static void SendToAll(char* buf, int len);

public:
	static std::vector<Connection*> clients;
	static std::vector< std::pair<Connection*, std::string > > nicknames;
};

#endif
