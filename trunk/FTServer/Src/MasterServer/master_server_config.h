/*****************************************************************************************
	filename:	master_server_config.h
	created:	09/27/2012
	author:		chen
	purpose:	additional configuration on master server

*****************************************************************************************/

#ifndef _H_MASTER_SERVER_CONFIG
#define _H_MASTER_SERVER_CONFIG

#include "server_config.h"
#include <map>

struct GateConfigItem
{
	uint8 m_iServerId;
	uint32 m_iServerIP;
	uint16 m_iServerPort;
	uint16 m_iSessionMax;
};

class MasterServerConfig : public ServerConfig
{
public:
	MasterServerConfig(const TCHAR* strServerName);
	
	// get gate server basic config by gate server id
	GateConfigItem* GetGateConfigItem(uint8 iServerId);

private:
	bool _LoadConfig();

public:
	std::map<uint8, GateConfigItem> m_mGateConfigItems;
};

#endif
