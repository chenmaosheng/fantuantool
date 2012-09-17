#include <cstdio>
#include "starnet.h"
#include "server.h"

int __cdecl main(int argc, char **argv)
{
	Server server;
	server.Init();
	printf("Listen to port: %d\n", 5150);
	
	while (true)
	{
		Sleep(1);
	}
	
	server.Destroy();

	return 0;
}
