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
};

#endif
