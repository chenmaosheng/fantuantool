#ifndef _H_COMMON_CONFIG
#define _H_COMMON_CONFIG

#include "server_common.h"
#include "tinyxml.h"
#include <map>
#include <string>

struct ServerConfigItem
{
	TCHAR m_strServerName[MAX_PATH+1];
	uint16 m_iServerId;
	uint32 m_iPeerIP;
	uint16 m_iPeerPort;
	TCHAR m_strConfigFile[MAX_PATH+1];
	TCHAR m_strExeFile[MAX_PATH+1];
};

class CommonConfig
{
public:
	CommonConfig();
	~CommonConfig();

	bool LoadConfig();

	ServerConfigItem* GetServerConfigItem(const TCHAR* strServerName);

public:
	TCHAR m_strLogPath[MAX_PATH+1];
	int32 m_iLogLevel;
	std::map<std::wstring, ServerConfigItem> m_mServerConfigItems;

private:
	TiXmlDocument m_XmlDoc;
};

#endif
