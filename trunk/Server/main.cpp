#include <cstdio>
#include "worker.h"
#include "server.h"

int __cdecl main(int argc, char **argv)
{
	WSADATA			wsd;
	
	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
	{
		fprintf(stderr, "unable to load Winsock!\n");
		return -1;
	}
	
	Server server;
	server.Init(5150);
	printf("Listen to port: %d\n", 5150);
	
	while (true)
	{
		server.Start();
	}
	
	WSACleanup();
	return 0;
}
