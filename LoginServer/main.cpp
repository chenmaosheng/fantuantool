#include "login_server.h"
#include "log.h"
#include "util.h"

int __cdecl _tmain(int argc, TCHAR **argv)
{
	if (argc < 2)
	{
		return -1;
	}

	g_pServer = LoginServer::Instance();
	g_pServer->Init(argv[1]);

	LOG_STT(LOG_SERVER, _T("Listen to port: %d"), 5150);

	while (true)
	{
		Sleep(1000);
	}

	g_pServer->Shutdown();
	g_pServer->Destroy();

	return 0;
}
