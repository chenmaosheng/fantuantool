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

private:
	bool _LoadConfig();

private:
	std::map<uint16, GateConfigItem> m_mGateConfigItems;
};

#endif
