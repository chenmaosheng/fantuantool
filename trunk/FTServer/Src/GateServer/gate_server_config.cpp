#include "gate_server_config.h"

GateServerConfig::GateServerConfig(const TCHAR* strServerName) :
ServerConfig(strServerName)
{

}

GateServerConfig::~GateServerConfig()
{

}

bool GateServerConfig::_LoadConfig()
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
	m_iThreadCount = atoi(pServerElement->Attribute("ThreadCount"));

	// report interval
	m_iReportInterval = (uint32)atoi(pServerElement->Attribute("ReportInterval"));

	TiXmlElement* pSessionElement = pRootElement->FirstChildElement("Session");
	if (!pSessionElement)
	{
		_ASSERT(false);
		return false;
	}

	// session max
	m_iSessionMax = (uint16)atoi(pSessionElement->Attribute("Max"));

	// check session interval
	m_iCheckSessionInterval = (uint32)atoi(pSessionElement->Attribute("CheckSessionInterval"));

	// connection timeout
	m_iConnectionTimeout = (uint32)atoi(pSessionElement->Attribute("ConnectionTimeout"));

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