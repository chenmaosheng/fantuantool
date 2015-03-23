#include "easy_net.h"
#include "easy_connection.h"
#include <vector>
#include "easy_acceptor.h"
#include "easy_worker.h"


int main(int argc, char* argv[])
{
	std::vector<EasyConnection*> ConnectionList;
	InitNet();
	EasyAcceptor* pAcceptor = CreateAcceptor(0, 9001);
	StartWorker(pAcceptor);

	while (true)
	{
		Sleep(100);
	}

	return 0;
}
