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
	TiXmlElement* pRootElement = m_XmlDoc.FirstChildElement("Config");
	if (!pRootElement)
	{
		return false;
	}

	TiXmlElement* pServerElement = pRootElement->FirstChildElement("Server");
	if (!pServerElement)
	{
		return false;
	}

	// server ip
	m_iServerIP = inet_addr(pServerElement->Attribute("IP"));

	// server port
	m_iServerPort = (uint16)atoi(pServerElement->Attribute("Port"));

	// thread count
	m_iThreadCount = atoi(pServerElement->Attribute("ThreadCount"));

	TiXmlElement* pSessionElement = pRootElement->FirstChildElement("Session");
	if (!pSessionElement)
	{
		return false;
	}

	// session max
	m_iSessionMax = (uint16)atoi(pSessionElement->Attribute("Max"));

	return true;
}