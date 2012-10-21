/*****************************************************************************************
filename:	cache_server_config.h
created:	09/27/2012
author:		chen
purpose:	additional configuration on cache server

*****************************************************************************************/

#ifndef _H_CACHE_SERVER_CONFIG
#define _H_CACHE_SERVER_CONFIG

#include "server_config.h"

class CacheServerConfig : public ServerConfig
{
public:
	CacheServerConfig(const TCHAR* strServerName);

private:
	bool _LoadConfig();

public:
	char m_strDBName[MAX_PATH];
	char m_strDBHost[MAX_PATH];
	char m_strDBUser[MAX_PATH];
	char m_strDBPassword[MAX_PATH];
	uint16 m_iDBPort;
	uint16 m_iDBConnCount;
	uint32 m_iReportInterval;	// interval of reporting to master server
};

#endif
