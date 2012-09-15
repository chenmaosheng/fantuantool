#ifndef _H_SERVER
#define _H_SERVER

#include "common.h"
#include "handler.h"
#include <string>
#include <vector>

struct Connection;
class Worker;
class Acceptor;
class ContextPool;
class Server
{
public:
	Server();
	virtual ~Server();

	void Init(uint16 port);
	void Start();

	static bool CALLBACK OnConnection(ConnID connID);
	static void CALLBACK OnDisconnect(ConnID connID);
	static void CALLBACK OnData(ConnID connID, uint16 iLen, char* pBuf);
	static void CALLBACK OnConnectFailed(void*);

	static std::string GetNickName(Connection* pConnection);
	static void DeleteClient(Connection* pConnection);

	static void SendToAll(char* buf, int len);

public:
	Acceptor* acceptor_;
	Handler handler_;
	Worker* worker_;
	ContextPool* context_pool_;
	static std::vector<Connection*> clients;
	static std::vector< std::pair<Connection*, std::string > > nicknames;
};

#endif
