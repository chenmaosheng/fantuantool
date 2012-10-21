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
	uint32 m_iReportInterval;	// interval of reporting to master server
	uint16 m_iSessionMax;
	uint32 m_iCheckSessionInterval;	// check if some sessions timeout
	uint32 m_iConnectionTimeout; // long time connection doesn't go next
	uint32 m_iLoginTimeout; // long time connection doesn't go next
	TCHAR m_strPrivateKey[MAX_PATH+1];
};

#endif
