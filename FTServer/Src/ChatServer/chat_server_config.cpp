#include "chat_server_config.h"

ChatServerConfig::ChatServerConfig(const TCHAR* strServerName) :
ServerConfig(strServerName)
{

}

ChatServerConfig::~ChatServerConfig()
{

}

bool ChatServerConfig::_LoadConfig()
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

	// check session interval
	m_iCheckSessionInterval = (uint32)atoi(pSessionElement->Attribute("CheckSessionInterval"));

	// connection timeout
	m_iConnectionTimeout = (uint32)atoi(pSessionElement->Attribute("ConnectionTimeout"));

	// chat timeout
	m_iChatTimeout = (uint32)atoi(pSessionElement->Attribute("ChatTimeout"));

	return true;
}