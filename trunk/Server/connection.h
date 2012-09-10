#ifndef _H_CONNECTION
#define _H_CONNECTION

struct Connection
{
	SOCKET socket;
	int index;
	char nickname[64];
};

#endif
