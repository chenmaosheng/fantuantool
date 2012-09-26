#ifndef _H_SERVER_CONFIG
#define _H_SERVER_CONFIG

#include "server_common.h"
#include "tinyxml.h"

class CommonConfig;
struct ServerConfigItem;
class ServerConfig
{
public:
	ServerConfig(const TCHAR* strServerName);
	virtual ~ServerConfig();

	bool LoadConfig();

	ServerConfigItem* GetServerConfigItemById(uint16 iServerId);
	ServerConfigItem* GetServerConfigItem(const TCHAR* strServerName);

private:
	virtual bool _LoadConfig() = 0;

public:
	TCHAR m_strServerName[MAX_PATH+1];
	uint16 m_iServerId;
	uint32 m_iPeerIP;
	uint16 m_iPeerPort;

private:
	CommonConfig* m_pCommonConfig;

protected:
	TiXmlDocument m_XmlDoc;
};

#endif
