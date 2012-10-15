/*****************************************************************************************
	filename:	server_config.h
	created:	09/27/2012
	author:		chen
	purpose:	basic server configuration for all servers

*****************************************************************************************/

#ifndef _H_SERVER_CONFIG
#define _H_SERVER_CONFIG

#include "server_common.h"
#include "common_config.h"
#include "tinyxml.h"

class ServerConfig
{
public:
	ServerConfig(const TCHAR* strServerName);
	virtual ~ServerConfig();

	// load server configuration
	bool LoadConfig();

	// get server basic configuration by id or name
	ServerConfigItem* GetServerConfigItemById(uint8 iServerId);
	ServerConfigItem* GetServerConfigItem(const TCHAR* strServerName);

	std::map<std::wstring, ChannelConfigItem>& GetChannelConfigItems();

	// get log configuration
	int32 GetLogLevel() const;
	const TCHAR* GetLogPath() const;

	// get misc issue
	uint16 GetGateSessionIndexMax() const;

private:
	// need implemented by each server
	virtual bool _LoadConfig() = 0;

public:
	TCHAR m_strServerName[MAX_PATH+1];
	uint8 m_iServerId;
	uint32 m_iPeerIP;
	uint16 m_iPeerPort;

private:
	CommonConfig m_CommonConfig;

protected:
	TiXmlDocument m_XmlDoc;
};

#endif
