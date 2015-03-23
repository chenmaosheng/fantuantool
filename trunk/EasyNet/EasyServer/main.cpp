#include "easy_server.h"

int __cdecl _tmain(int argc, TCHAR **argv)
{
	g_pServer = new EasyServer;
	g_pServer->Init();

	while (true)
    {
		Sleep(1000);
    }

	g_pServer->Shutdown();
	g_pServer->Destroy();

    return 0;
}
