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
	uint16 m_iServerId;
	uint32 m_iServerIP;
	uint16 m_iServerPort;
	uint16 m_iSessionMax;
};

class MasterServerConfig : public ServerConfig
{
public:
	MasterServerConfig(const TCHAR* strServerName);
	~MasterServerConfig();

	GateConfigItem* GetGateConfigItem(uint16 iServerId);

private:
	bool _LoadConfig();

private:
	std::map<uint16, GateConfigItem> m_mGateConfigItems;
};

#endif
