#include "cache_server_config.h"

CacheServerConfig::CacheServerConfig(const TCHAR* strServerName) :
ServerConfig(strServerName)
{

}

bool CacheServerConfig::_LoadConfig()
{
	TiXmlElement* pRootElement = m_XmlDoc.FirstChildElement("Config");
	if (!pRootElement)
	{
		return false;
	}

	TiXmlElement* pCacheServerListElement = pRootElement->FirstChildElement("CacheServer");
	if (!pCacheServerListElement)
	{
		return false;
	}

	// db name
	strcpy_s(m_strDBName, MAX_PATH, pCacheServerListElement->Attribute("DBName"));

	// db ip
	strcpy_s(m_strDBHost, MAX_PATH, pCacheServerListElement->Attribute("DBHost"));

	// db user
	strcpy_s(m_strDBUser, MAX_PATH, pCacheServerListElement->Attribute("DBUser"));

	// db password
	strcpy_s(m_strDBPassword, MAX_PATH, pCacheServerListElement->Attribute("DBPassword"));

	// db port
	m_iDBPort = (uint16)atoi(pCacheServerListElement->Attribute("DBPort"));

	// db conn count
	m_iDBConnCount = (uint16)atoi(pCacheServerListElement->Attribute("DBConnCount"));

	// report interval
	m_iReportInterval = (uint32)atoi(pCacheServerListElement->Attribute("ReportInterval"));

	return true;
}