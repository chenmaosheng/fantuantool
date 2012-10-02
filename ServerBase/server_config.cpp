#include "server_config.h"
#include "common_config.h"

ServerConfig::ServerConfig(const TCHAR* strServerName)
{
	m_pCommonConfig = new CommonConfig;
	m_strServerName[0] = _T('\0');
	m_iServerId = 0;
	m_iPeerIP = 0;
	m_iPeerPort = 0;

	wcscpy_s(m_strServerName, _countof(m_strServerName), strServerName);
}

ServerConfig::~ServerConfig()
{
	SAFE_DELETE(m_pCommonConfig);
}

bool ServerConfig::LoadConfig()
{
	int32 iRet = 0;
	char serverConfigFile[MAX_PATH + 1] = {0};
	char fullServerConfigFile[MAX_PATH + 1] = {0};

	if (!m_pCommonConfig->LoadConfig())
	{
		return false;
	}

	ServerConfigItem* pServerConfigItem = m_pCommonConfig->GetServerConfigItem(m_strServerName);
	if (!pServerConfigItem)
	{
		return false;
	}

	// server startup configuration
	m_iServerId = pServerConfigItem->m_iServerId;
	m_iPeerIP = pServerConfigItem->m_iPeerIP;
	m_iPeerPort = pServerConfigItem->m_iPeerPort;

	iRet = WChar2Char(pServerConfigItem->m_strConfigFile, serverConfigFile, MAX_PATH + 1);
	if (iRet == 0)
	{
		return false;
	}
	serverConfigFile[iRet] = '\0';

	_snprintf_s(fullServerConfigFile, MAX_PATH + 1, "%s\\%s.xml", CONFIG_PATH, serverConfigFile);

	// load server's own config file
	if (!m_XmlDoc.LoadFile(fullServerConfigFile))
	{
		return false;
	}

	return _LoadConfig();
}

ServerConfigItem* ServerConfig::GetServerConfigItemById(uint16 iServerId)
{
	return m_pCommonConfig->GetServerConfigItemById(iServerId);
}

ServerConfigItem* ServerConfig::GetServerConfigItem(const TCHAR *strServerName)
{
	return m_pCommonConfig->GetServerConfigItem(strServerName);
}

int32 ServerConfig::GetLogLevel() const
{
	return m_pCommonConfig->GetLogLevel();
}

const TCHAR* ServerConfig::GetLogPath() const
{
	return m_pCommonConfig->GetLogPath();
}