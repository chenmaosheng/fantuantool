#include "region_server_config.h"

RegionServerConfig::RegionServerConfig(const TCHAR* strServerName) :
ServerConfig(strServerName)
{

}

bool RegionServerConfig::_LoadConfig()
{
	TiXmlElement* pRootElement = m_XmlDoc.FirstChildElement("Config");
	if (!pRootElement)
	{
		return false;
	}

	TiXmlElement* pRegionServerElement = pRootElement->FirstChildElement("RegionServer");
	if (!pRegionServerElement)
	{
		return false;
	}

	// player max
	m_iPlayerMax = (uint16)atoi(pRegionServerElement->Attribute("PlayerMax"));

	TiXmlElement* pRegionDescListElement = pRootElement->FirstChildElement("RegionDescList");
	if (!pRegionDescListElement)
	{
		return false;
	}

	for (TiXmlElement* pRegionDescElement = pRegionDescListElement->FirstChildElement("RegionDesc");
		pRegionDescElement != NULL; pRegionDescElement = pRegionDescElement->NextSiblingElement("RegionDesc"))
	{
		RegionDesc desc;

		// instance count
		desc.m_iInstanceCount = (uint16)atoi(pRegionDescElement->Attribute("InstanceCount"));

		// map id list
		TiXmlElement* pMapListElement = pRegionDescElement->FirstChildElement("MapIdList");
		if (!pMapListElement)
		{
			return false;
		}

		for (TiXmlElement* pMapElement = pMapListElement->FirstChildElement("MapId"); pMapElement != NULL; pMapElement = pMapElement->NextSiblingElement("Map"))
		{
			desc.m_arrayMapList.push_back((uint32)atoi(pMapElement->Attribute("value")));
		}

		m_vRegionDesc.push_back(desc);
	}

	TiXmlElement* pSpawnPointListElement = pRootElement->FirstChildElement("SpawnPointList");
	if (!pSpawnPointListElement)
	{
		return false;
	}

	for (TiXmlElement* pSpawnPointElement = pSpawnPointListElement->FirstChildElement("SpawnPoint"); pSpawnPointElement != NULL;
		pSpawnPointElement = pSpawnPointElement->NextSiblingElement("SpawnPoint"))
	{
		SpawnPointDesc desc;

		// map id
		desc.m_iMapId = (uint32)atoi(pSpawnPointElement->Attribute("MapId"));
		desc.m_iPointId = (uint32)atoi(pSpawnPointElement->Attribute("PointId"));

		m_vSpawnPointDesc.push_back(desc);
	}

	return true;
}
