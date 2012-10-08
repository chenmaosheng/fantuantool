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
	// load server configuration
	bool LoadConfig();

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

	const TCHAR* GetCreateAccountPage() const
	{
		return m_strCreateAccountPage;
	}

private:
	TCHAR m_strLogPath[MAX_PATH+1];
	int32 m_iLogLevel;
	char m_strLoginHost[32];
	uint16 m_iLoginPort;
	TCHAR m_strCreateAccountPage[MAX_PATH+1];

	TiXmlDocument m_XmlDoc;
};

#endif
