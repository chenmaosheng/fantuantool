/*****************************************************************************************
	filename:	common_config.h
	created:	09/27/2012
	author:		chen
	purpose:	common configuration for all servers

*****************************************************************************************/

#ifndef _H_COMMON_CONFIG
#define _H_COMMON_CONFIG

#include "server_common.h"
#include "tinyxml.h"
#include <map>
#include <string>

// record the basic configuration of one server
struct ServerConfigItem
{
	TCHAR m_strServerName[MAX_PATH+1];	// server name
	uint8 m_iServerId;					// server id, not allowed to duplicate, will check at config verification function(todo:)
	uint32 m_iPeerIP;					// IP when serve as a peer server
	uint16 m_iPeerPort;					// port when serve as a peer server
	TCHAR m_strConfigFile[MAX_PATH+1];	// the config file name of this server
	TCHAR m_strExeFile[MAX_PATH+1];		// the executable file name of this server
};

class CommonConfig
{
public:
	CommonConfig();
	
	// load common configuration from assigned folder
	bool LoadConfig();

	// get server basic configuration
	ServerConfigItem* GetServerConfigItem(const TCHAR* strServerName);
	ServerConfigItem* GetServerConfigItemById(uint8 iServerId);

	int32 GetLogLevel() const
	{
		return m_iLogLevel;
	}

	const TCHAR* GetLogPath() const
	{
		return m_strLogPath;
	}

private:
	TCHAR m_strLogPath[MAX_PATH+1];
	int32 m_iLogLevel;
	std::map<std::wstring, ServerConfigItem> m_mServerConfigItems;
	TiXmlDocument m_XmlDoc;
};

#endif
