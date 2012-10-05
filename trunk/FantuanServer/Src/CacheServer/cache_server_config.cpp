#include "cache_server_config.h"

CacheServerConfig::CacheServerConfig(const TCHAR* strServerName) :
ServerConfig(strServerName)
{

}

bool CacheServerConfig::_LoadConfig()
{
	return true;
}