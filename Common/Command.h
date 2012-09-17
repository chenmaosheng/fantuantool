#ifndef _H_COMMAND
#define _H_COMMAND

enum
{
	LOGIN = 1,
	LOGOUT,
	SEND_MESSAGE,
};

template<unsigned short cmdId>
class Command
{
public:
	static const unsigned short ID = cmdId;

	Command()
	{
		m_iCmdId = ID;
	}

	virtual ~Command()
	{
	}

public:
	int		m_iCmdId;
};

class LoginCommand : public Command<LOGIN>
{
public:
	LoginCommand()
	{
	}

	~LoginCommand()
	{
	}

public:
};

class LogoutCommand : public Command<LOGOUT>
{
public:
	LogoutCommand()
	{
	}

	~LogoutCommand()
	{
	}

public:
};

class SendMessageCommand : public Command<SEND_MESSAGE>
{
public:
	SendMessageCommand()
	{
	}

	~SendMessageCommand()
	{
	}

public:
};

struct Header
{
	char type;
	int  len;

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
	char message[256];
};

#endif
