#include "region_server.h"

int __cdecl _tmain(int argc, TCHAR **argv)
{
	if (argc < 2)
	{
		return -1;
	}

	g_pServer = RegionServer::Instance();
	g_pServer->Init(argv[1]);

	while (true)
	{
		Sleep(1000);
	}

	g_pServer->Shutdown();
	g_pServer->Destroy();

	return 0;
}
