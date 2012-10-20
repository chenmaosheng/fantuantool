#include "login_server_config.h"

LoginServerConfig::LoginServerConfig(const TCHAR* strServerName) :
ServerConfig(strServerName)
{

}

LoginServerConfig::~LoginServerConfig()
{

}

bool LoginServerConfig::_LoadConfig()
{
	int32 iRet = 0;

	TiXmlElement* pRootElement = m_XmlDoc.FirstChildElement("Config");
	if (!pRootElement)
	{
		_ASSERT(false);
		return false;
	}

	TiXmlElement* pServerElement = pRootElement->FirstChildElement("Server");
	if (!pServerElement)
	{
		_ASSERT(false);
		return false;
	}

	// server ip
	m_iServerIP = inet_addr(pServerElement->Attribute("IP"));

	// server port
	m_iServerPort = (uint16)atoi(pServerElement->Attribute("Port"));

	// thread count
	m_iThreadCount = (uint32)atoi(pServerElement->Attribute("ThreadCount"));

	TiXmlElement* pSessionElement = pRootElement->FirstChildElement("Session");
	if (!pSessionElement)
	{
		_ASSERT(false);
		return false;
	}

	// session max
	m_iSessionMax = (uint16)atoi(pSessionElement->Attribute("Max"));

	TiXmlElement* pSecurityElement = pRootElement->FirstChildElement("Security");
	if (!pSecurityElement)
	{
		_ASSERT(false);
		return false;
	}

	// private key
	iRet = Char2WChar(pSecurityElement->Attribute("PrivateKey"), m_strPrivateKey, MAX_PATH+1);
	if (iRet == 0)
	{
		_ASSERT(false);
		return false;
	}
	m_strPrivateKey[iRet] = _T('\0');

	return true;
}