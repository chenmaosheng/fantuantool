#include "db_conn.h"
#include <process.h>

DBConn::DBConn()
{
	m_pMySQL = NULL;
	m_hThread = NULL;
	m_bQuit = false;
}

void DBConn::Init()
{

}

void DBConn::Destroy()
{
	if (m_pMySQL)
	{
		mysql_close(m_pMySQL);
	}
}

void DBConn::Start()
{
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, &DBConn::_HandleDBConn, this, 0, NULL);
}

void DBConn::Stop()
{
	m_bQuit = true;
}

void DBConn::Join()
{
	WaitForSingleObject(m_hThread, INFINITE);
}

bool DBConn::_InitConnector()
{
	m_pMySQL = mysql_init(NULL);
	if (m_pMySQL)
	{
		//if (mysql_real_connect(m_pMySQL, 
	}

	return false;
}

uint32 WINAPI DBConn::_HandleDBConn(PVOID pParam)
{
	return 0;
}