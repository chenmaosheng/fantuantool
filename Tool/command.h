#ifndef _H_COMMAND
#define _H_COMMAND

enum
{
	LOGIN,
	LOGOUT,
	SEND_MESSAGE,
};

struct Header
{
	unsigned short len;
	unsigned short type;

	Header()
	{
		len = 0;
	}
};

struct LoginPkt : public Header
{
	LoginPkt()
	{
		type = LOGIN;
		memset(nickname, 0, sizeof(nickname));
		connId = 0;
	}
	int connId;
	char nickname[64];
};

struct LogoutPkt : public Header
{
	LogoutPkt()
	{
		type = LOGOUT;
		connId = 0;
	}
	int connId;
};

struct SendMessagePkt : public Header
{
	SendMessagePkt()
	{
		type = SEND_MESSAGE;
		memset(message, 0, sizeof(message));
	}
	char message[1024];
};

#endif
