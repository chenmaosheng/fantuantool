/*****************************************************************************************
	filename:	chat_server_config.h
	created:	12/13/2014
	author:		chen
	purpose:	additional configuration on chat server

*****************************************************************************************/

#ifndef _H_CHAT_SERVER_CONFIG
#define _H_CHAT_SERVER_CONFIG

#include "server_config.h"

class ChatServerConfig : public ServerConfig
{
public:
	ChatServerConfig(const TCHAR* strServerName);
	~ChatServerConfig();

private:
	bool _LoadConfig();

public:
	uint32 m_iServerIP;
	uint16 m_iServerPort;
	uint32 m_iThreadCount;
	uint16 m_iSessionMax;
	uint32 m_iCheckSessionInterval;	// check if some sessions timeout
	uint32 m_iConnectionTimeout; // long time connection doesn't go next
	uint32 m_iChatTimeout; // long time connection doesn't go next
};

#endif
