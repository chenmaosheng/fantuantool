#include "client_config.h"

bool ClientConfig::LoadConfig()
{
	int32 iRet = 0;
	char commonConfigFile[MAX_PATH + 1] = {0};

	// combine the filename and load xml file
	_snprintf_s(commonConfigFile, MAX_PATH + 1, "%s\\%s.xml", CONFIG_PATH, CLIENT_CONFIG_FILE);
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

	TiXmlElement* pLoginElement = pRootElement->FirstChildElement("Login");
	if (!pLoginElement)
	{
		return false;
	}

	// get login server's ip
	strcpy_s(m_strLoginHost, sizeof(m_strLoginHost), pLoginElement->Attribute("Host"));
	
	// get login server's port
	m_iLoginPort = (uint16)atoi(pLoginElement->Attribute("Port"));

	// get user name
	iRet = Char2WChar(pLoginElement->Attribute("Account"), m_strAccountName, ACCOUNTNAME_MAX+1);
	if (iRet == 0)
	{
		return false;
	}
	m_strAccountName[iRet] = _T('\0');

	// get password
	iRet = Char2WChar(pLoginElement->Attribute("Password"), m_strPassword, PASSWORD_MAX+1);
	if (iRet == 0)
	{
		return false;
	}
	m_strPassword[iRet] = _T('\0');

	TiXmlElement* pConnectionServiceElement = pRootElement->FirstChildElement("ConnectionService");
	if (!pConnectionServiceElement)
	{
		return false;
	}

	iRet = Char2WChar(pConnectionServiceElement->Attribute("CreateAccountPage"), m_strCreateAccountPage, MAX_PATH + 1);
	if (iRet == 0)
	{
		return false;
	}

	m_strCreateAccountPage[iRet] = _T('\0');

	return true;
}

void ClientConfig::SaveConfig(const TCHAR *strAccountName, const TCHAR *strPassword)
{
	int32 iRet = 0;

	TiXmlElement* pRootElement = m_XmlDoc.FirstChildElement("Config");
	if (!pRootElement)
	{
		return;
	}

	TiXmlElement* pLoginElement = pRootElement->FirstChildElement("Login");
	if (!pLoginElement)
	{
		return;
	}

	// set user name
	char strUtf8[ACCOUNTNAME_MAX+1] = {0};
	iRet = WChar2Char(strAccountName, strUtf8, ACCOUNTNAME_MAX+1);
	if (iRet == 0)
	{
		return;
	}
	strUtf8[iRet] = '\0';
	pLoginElement->SetAttribute("Account", strUtf8);
	
	// set password
	char strUtf82[PASSWORD_MAX+1] = {0};
	iRet = WChar2Char(strPassword, strUtf82, PASSWORD_MAX+1);
	if (iRet == 0)
	{
		return;
	}
	strUtf82[iRet] = '\0';
	pLoginElement->SetAttribute("Password", strUtf82);

	m_XmlDoc.SaveFile();
}