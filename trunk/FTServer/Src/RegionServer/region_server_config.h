/*****************************************************************************************
	filename:	region_server_config.h
	created:	10/14/2012
	author:		chen
	purpose:	additional configuration on region server

*****************************************************************************************/

#ifndef _H_REGION_SERVER_CONFIG
#define _H_REGION_SERVER_CONFIG

#include "server_config.h"
#include <vector>

struct RegionDesc
{
	uint16 m_iInstanceCount;	// max instance count in this region
	std::vector<uint16> m_arrayMapList; // list of map in this region
};

struct SpawnPointDesc
{
	uint16 m_iMapId;
	uint16 m_iPointId;
};

class RegionServerConfig : public ServerConfig
{
public:
	RegionServerConfig(const TCHAR* strServerName);
	
private:
	bool _LoadConfig();

public:
	uint16 m_iPlayerMax;
	std::vector<RegionDesc> m_vRegionDesc;
	std::vector<SpawnPointDesc> m_vSpawnPointDesc;
};

#endif
