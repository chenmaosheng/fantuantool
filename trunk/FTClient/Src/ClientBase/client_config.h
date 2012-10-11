/*****************************************************************************************
	filename:	client_config.h
	created:	10/08/2012
	author:		chen
	purpose:	client configuration

*****************************************************************************************/

#ifndef _H_CLIENT_CONFIG
#define _H_CLIENT_CONFIG

#include "client_common.h"
#include "tinyxml.h"

class ClientConfig
{
public:
	// load client configuration
	bool LoadConfig();
	// save client configuration
	void SaveConfig(const TCHAR* strAccountName, const TCHAR* strPassword);

	// get log configuration
	int32 GetLogLevel() const
	{
		return m_iLogLevel;
	}

	const TCHAR* GetLogPath() const
	{
		return m_strLogPath;
	}

	uint16 GetLoginPort() const
	{
		return m_iLoginPort;
	}

	const char* GetLoginHost() const
	{
		return m_strLoginHost;
	}

	const TCHAR* GetAcccountName() const
	{
		return m_strAccountName;
	}

	const TCHAR* GetPassword() const
	{
		return m_strPassword;
	}

	const TCHAR* GetCreateAccountPage() const
	{
		return m_strCreateAccountPage;
	}

private:
	TCHAR m_strLogPath[MAX_PATH+1];
	int32 m_iLogLevel;
	char m_strLoginHost[32];
	uint16 m_iLoginPort;
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
	TCHAR m_strPassword[PASSWORD_MAX+1];
	TCHAR m_strCreateAccountPage[MAX_PATH+1];

	TiXmlDocument m_XmlDoc;
};

#endif
