#include "common_config.h"

CommonConfig::CommonConfig()
{
	m_strLogPath[0] = _T('\0');
	m_iLogLevel = Log::LOG_DEBUG_LEVEL;
	m_iGateSessionIndexMax = 0;
}

bool CommonConfig::LoadConfig()
{
	int32 iRet = 0;
	char commonConfigFile[MAX_PATH + 1] = {0};

	// combine the filename and load xml file
	_snprintf_s(commonConfigFile, MAX_PATH + 1, "%s\\%s.xml", CONFIG_PATH, COMMON_CONFIG_FILE);
	if (!m_XmlDoc.LoadFile(commonConfigFile))
	{
		_ASSERT(false);
		return false;
	}

	TiXmlElement* pRootElement = m_XmlDoc.FirstChildElement("Config");
	_ASSERT(pRootElement);
	if (!pRootElement)
	{
		return false;
	}

	TiXmlElement* pLogElement = pRootElement->FirstChildElement("Log");
	_ASSERT(pLogElement);
	if (!pLogElement)
	{
		return false;
	}

	// get log path
	iRet = Char2WChar(pLogElement->Attribute("Path"), m_strLogPath, MAX_PATH + 1);
	_ASSERT(iRet);
	if (iRet == 0)
	{
		return false;
	}

	m_strLogPath[iRet] = _T('\0');

	// get log level
	m_iLogLevel = atoi(pLogElement->Attribute("Level"));

	TiXmlElement* pMiscElement = pRootElement->FirstChildElement("Misc");
	_ASSERT(pMiscElement);
	if (!pMiscElement)
	{
		return false;
	}

	// get gate session_index max value
	m_iGateSessionIndexMax = (uint16)atoi(pMiscElement->Attribute("GateSessionIndexMax"));

	TiXmlElement* pRealmElement = pRootElement->FirstChildElement("Realm");
	_ASSERT(pRealmElement);
	if (!pRealmElement)
	{
		return false;
	}

	TiXmlElement* pServerListElement = pRealmElement->FirstChildElement("ServerList");
	_ASSERT(pServerListElement);
	if (!pServerListElement)
	{
		return false;
	}

	for (TiXmlElement* pServerElement = pServerListElement->FirstChildElement("Server"); pServerElement != NULL; pServerElement = pServerElement->NextSiblingElement("Server"))
	{
		ServerConfigItem item;
		memset(&item, 0, sizeof(item));
		// server name
		iRet = Char2WChar(pServerElement->Attribute("name"), item.m_strServerName, SERVERNAME_MAX + 1);
		_ASSERT(iRet);
		if (iRet == 0)
		{
			return false;
		}

		item.m_strServerName[iRet] = _T('\0');
		
		// server id
		item.m_iServerId = (uint8)atoi(pServerElement->Attribute("id"));

		// peer ip
		item.m_iPeerIP = inet_addr(pServerElement->Attribute("PeerIP"));

		// peer port
		item.m_iPeerPort = (uint16)atoi(pServerElement->Attribute("PeerPort"));

		// config file
		iRet = Char2WChar(pServerElement->Attribute("config"), item.m_strConfigFile, MAX_PATH + 1);
		_ASSERT(iRet);
		if (iRet == 0)
		{
			return false;
		}

		item.m_strConfigFile[iRet] = _T('\0');

		// exe file
		iRet = Char2WChar(pServerElement->Attribute("exe"), item.m_strExeFile, MAX_PATH + 1);
		_ASSERT(iRet);
		if (iRet == 0)
		{
			return false;
		}

		item.m_strExeFile[iRet] = _T('\0');

		m_mServerConfigItems.insert(std::make_pair(item.m_strServerName, item));
	}

	TiXmlElement* pChannelListElement = pRealmElement->FirstChildElement("ChannelList");
	_ASSERT(pChannelListElement);
	if (!pChannelListElement)
	{
		return false;
	}

	for (TiXmlElement* pChannelElement = pChannelListElement->FirstChildElement("Channel"); pChannelElement != NULL; pChannelElement = pChannelElement->NextSiblingElement("Channel"))
	{
		ChannelConfigItem item;
		memset(&item, 0, sizeof(item));
		// channel name
		iRet = Char2WChar(pChannelElement->Attribute("name"), item.m_strChannelName, CHANNELNAME_MAX+1);
		_ASSERT(iRet);
		if (iRet == 0)
		{
			return false;
		}

		item.m_strChannelName[iRet] = _T('\0');
		
		// channel id
		item.m_iChannelId = (uint8)atoi(pChannelElement->Attribute("id"));

		// player max
		item.m_iPlayerMax = (uint16)atoi(pChannelElement->Attribute("PlayerMax"));

		// initial region server id
		item.m_iInitialRegionServerId = (uint8)atoi(pChannelElement->Attribute("InitialRegion"));

		for (TiXmlElement* pRegionElement = pChannelElement->FirstChildElement("Region"); pRegionElement != NULL; pRegionElement = pRegionElement->NextSiblingElement("Region"))
		{
			// region id
			item.m_arrayRegionConfig[item.m_iRegionCount].m_iServerId = (uint8)atoi(pRegionElement->Attribute("id"));
			// player max
			item.m_arrayRegionConfig[item.m_iRegionCount].m_iPlayerMax = (uint8)atoi(pRegionElement->Attribute("PlayerMax"));
			item.m_arrayRegionConfig[item.m_iRegionCount].m_iChannelId = item.m_iChannelId;

			item.m_iRegionCount++;
		}

		m_mChannelConfigItems.insert(std::make_pair(item.m_strChannelName, item));
	}

	return true;
}

ServerConfigItem* CommonConfig::GetServerConfigItem(const TCHAR* strServerName)
{
	std::map<std::wstring, ServerConfigItem>::iterator mit = m_mServerConfigItems.find(strServerName);
	if (mit != m_mServerConfigItems.end())
	{
		return &(mit->second);
	}

	return NULL;
}

ServerConfigItem* CommonConfig::GetServerConfigItemById(uint8 iServerId)
{
	std::map<std::wstring, ServerConfigItem>::iterator mit = m_mServerConfigItems.begin();
	while (mit != m_mServerConfigItems.end())
	{
		if (mit->second.m_iServerId == iServerId)
		{
			return &(mit->second);
		}

		++mit;
	}

	return NULL;
}