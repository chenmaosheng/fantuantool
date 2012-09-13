#ifndef _H_WORKER
#define _H_WORKER

#include "common.h"
#include "connection.h"
#include <vector>

class Worker
{
public:
	Worker();
	~Worker();

	void Init();
	void Destroy();

	static uint32 WINAPI WorkerThread(PVOID);

	std::vector<Connection*> clients;
	std::vector< std::pair<Connection*, std::string > > nicknames;

	std::string GetNickName(Connection* pConnection);
	void DeleteClient(Connection* pConnection);

	void SendToAll(char* buf, int len);

public:
	HANDLE	iocp_;
};

#endif
