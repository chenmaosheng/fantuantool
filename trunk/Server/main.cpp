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
		server.Start();
	}
	
	server.Destroy();

	return 0;
}
