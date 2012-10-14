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

	TiXmlElement* pRegionDescElement = pRootElement->FirstChildElement("RegionDesc");
	if (!pRegionDescElement)
	{
		return false;
	}

	// instance count
	m_RegionDesc.m_iInstanceCount = (uint16)atoi(pRegionDescElement->Attribute("InstanceCount"));

	TiXmlElement* pMapListElement = pRegionDescElement->FirstChildElement("MapList");
	if (!pMapListElement)
	{
		return false;
	}

	for (TiXmlElement* pMapElement = pMapListElement->FirstChildElement("Map"); pMapElement != NULL; pMapElement = pMapElement->NextSiblingElement("Map"))
	{
		m_RegionDesc.m_arrayMapList.push_back((uint16)atoi(pMapElement->Attribute("id")));
	}
}
