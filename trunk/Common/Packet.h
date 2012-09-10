#ifndef _H_PACKET
#define _H_PACKET

struct Header
{
	char type;
	int  len;

	Header()
	{
		len = 0;
	}
};

enum
{
	LOGIN = 1,
	LOGOUT,
	SEND_MESSAGE,
};

struct LoginPkt : public Header
{
	LoginPkt()
	{
		type = LOGIN;
		memset(nickname, 0, sizeof(nickname));
		index = 0;
	}
	int index;
	char nickname[32];
};

struct LogoutPkt : public Header
{
	LogoutPkt()
	{
		type = LOGOUT;
		index = 0;
	}
	int index;
};

struct SendMessagePkt : public Header
{
	SendMessagePkt()
	{
		type = SEND_MESSAGE;
		memset(message, 0, sizeof(message));
	}
	char message[256];
};

#endif
