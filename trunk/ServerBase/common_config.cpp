#include "common_config.h"

CommonConfig::CommonConfig()
{
	m_strLogPath[0] = _T('\0');
	m_iLogLevel = Log::LOG_DEBUG_LEVEL;
}

bool CommonConfig::LoadConfig()
{
	int32 iRet = 0;
	char commonConfigFile[MAX_PATH + 1] = {0};

	// combine the filename and load xml file
	_snprintf_s(commonConfigFile, MAX_PATH + 1, "%s\\%s.xml", CONFIG_PATH, COMMON_CONFIG_FILE);
	if (!m_XmlDoc.LoadFile(commonConfigFile))
	{
		return false;
	}

	TiXmlElement* pRootElement = m_XmlDoc.FirstChildElement("Config");
	if (!pRootElement)
	{
		return false;
	}

	TiXmlElement* pLogElement = pRootElement->FirstChildElement("Log");
	if (!pLogElement)
	{
		return false;
	}

	// get log path
	iRet = Char2WChar(pLogElement->Attribute("Path"), m_strLogPath, MAX_PATH + 1);
	if (iRet == 0)
	{
		return false;
	}

	m_strLogPath[iRet] = _T('\0');

	// get log level
	m_iLogLevel = atoi(pLogElement->Attribute("Level"));

	TiXmlElement* pRealmElement = pRootElement->FirstChildElement("Realm");
	if (!pRealmElement)
	{
		return false;
	}

	TiXmlElement* pServerListElement = pRealmElement->FirstChildElement("ServerList");
	if (!pServerListElement)
	{
		return false;
	}

	for (TiXmlElement* pServerElement = pServerListElement->FirstChildElement("Server"); pServerElement != NULL; pServerElement = pServerElement->NextSiblingElement("Server"))
	{
		ServerConfigItem item;
		memset(&item, 0, sizeof(item));
		// server name
		iRet = Char2WChar(pServerElement->Attribute("name"), item.m_strServerName, MAX_PATH + 1);
		if (iRet == 0)
		{
			return false;
		}

		item.m_strServerName[iRet] = _T('\0');
		
		// server id
		item.m_iServerId = (uint16)atoi(pServerElement->Attribute("id"));

		// peer ip
		item.m_iPeerIP = inet_addr(pServerElement->Attribute("PeerIP"));

		// peer port
		item.m_iPeerPort = (uint16)atoi(pServerElement->Attribute("PeerPort"));

		// config file
		iRet = Char2WChar(pServerElement->Attribute("config"), item.m_strConfigFile, MAX_PATH + 1);
		if (iRet == 0)
		{
			return false;
		}

		item.m_strConfigFile[iRet] = _T('\0');

		// exe file
		iRet = Char2WChar(pServerElement->Attribute("exe"), item.m_strExeFile, MAX_PATH + 1);
		if (iRet == 0)
		{
			return false;
		}

		item.m_strExeFile[iRet] = _T('\0');

		m_mServerConfigItems.insert(std::make_pair(item.m_strServerName, item));
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

ServerConfigItem* CommonConfig::GetServerConfigItemById(uint16 iServerId)
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