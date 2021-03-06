#include "master_server_config.h"

MasterServerConfig::MasterServerConfig(const TCHAR* strServerName) :
ServerConfig(strServerName)
{

}

GateConfigItem* MasterServerConfig::GetGateConfigItem(uint8 iServerId)
{
	std::map<uint8, GateConfigItem>::iterator mit = m_mGateConfigItems.find(iServerId);
	if (mit != m_mGateConfigItems.end())
	{
		return &(mit->second);
	}

	return NULL;
}

bool MasterServerConfig::_LoadConfig()
{
	TiXmlElement* pRootElement = m_XmlDoc.FirstChildElement("Config");
	if (!pRootElement)
	{
		return false;
	}

	TiXmlElement* pGateServerListElement = pRootElement->FirstChildElement("GateServerList");
	if (!pGateServerListElement)
	{
		return false;
	}

	for (TiXmlElement* pGateServerElement = pGateServerListElement->FirstChildElement("GateServer"); pGateServerListElement != NULL; pGateServerListElement = pGateServerListElement->NextSiblingElement("GateServer"))
	{
		GateConfigItem item;
		memset(&item, 0, sizeof(item));

		// server id
		item.m_iServerId = (uint8)atoi(pGateServerElement->Attribute("id"));

		// server ip
		item.m_iServerIP = inet_addr(pGateServerElement->Attribute("ServerIP"));

		// server port
		item.m_iServerPort = (uint16)atoi(pGateServerElement->Attribute("ServerPort"));

		// session max
		item.m_iSessionMax = (uint16)atoi(pGateServerElement->Attribute("SessionMax"));

		m_mGateConfigItems.insert(std::make_pair(item.m_iServerId, item));
	}

	return true;
}