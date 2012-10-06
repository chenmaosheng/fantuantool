/*****************************************************************************************
	filename:	login_server_config.h
	created:	09/27/2012
	author:		chen
	purpose:	additional configuration on login server

*****************************************************************************************/

#ifndef _H_LOGIN_SERVER_CONFIG
#define _H_LOGIN_SERVER_CONFIG

#include "server_config.h"

class LoginServerConfig : public ServerConfig
{
public:
	LoginServerConfig(const TCHAR* strServerName);
	~LoginServerConfig();

private:
	bool _LoadConfig();

public:
	uint32 m_iServerIP;
	uint16 m_iServerPort;
	uint32 m_iThreadCount;
	uint16 m_iSessionMax;
};

#endif
