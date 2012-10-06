#include "db_conn.h"
#include "db_conn_pool.h"
#include "db_event.h"
#include <process.h>

DBConn::DBConn()
{
	m_pMySQL = NULL;
	m_hThread = NULL;
	m_bQuit = false;
	m_pDBConnPool = NULL;
}

void DBConn::Init(DBConnPool* pDBConnPool)
{
	m_pDBConnPool = pDBConnPool;
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

MYSQL* DBConn::GetConnector()
{
	return m_pMySQL;
}

void DBConn::DeleteConnector()
{
	mysql_close(m_pMySQL);
	m_pMySQL = NULL;
}

bool DBConn::_InitConnector()
{
	m_pMySQL = mysql_init(NULL);
	if (m_pMySQL)
	{
		// try to connect to mysql
		if (mysql_real_connect(m_pMySQL, m_pDBConnPool->m_strHost, m_pDBConnPool->m_strUserName, 
			m_pDBConnPool->m_strPassword, NULL, m_pDBConnPool->m_iPort, NULL, 0))
		{
			// choose required database
			if (mysql_select_db(m_pMySQL, m_pDBConnPool->m_strDBName) == 0)
			{
				LOG_STT(LOG_DB, _T("Connect to db success"));
				return true;
			}

			mysql_close(m_pMySQL);
		}
	}

	LOG_ERR(LOG_DB, _T("Connect to db failed"));

	return false;
}

uint32 WINAPI DBConn::_HandleDBConn(PVOID pParam)
{
	DBConn* pDBConn = (DBConn*)pParam;
	DBEvent* pEvent = NULL;
	uint64 iLastEventSequenceId = 0;
	int32 iRet = 0;

	while (!pDBConn->m_bQuit)
	{
		if (!pDBConn->GetConnector())
		{
			if (!pDBConn->_InitConnector())
			{
				// if connect failed, try it again later
				Sleep(2000);
				continue;
			}
		}

		// pop a event
		pEvent = pDBConn->m_pDBConnPool->PopFromDBEventProcessingList(iLastEventSequenceId);
		if (!pEvent)
		{
			continue;
		}

		iLastEventSequenceId = pEvent->m_iSequenceId;

		// fire event
		iRet = pEvent->FireEvent(pDBConn);
		if (iRet < 0)
		{
			pDBConn->DeleteConnector();
		}

		pEvent->m_bFired = true;

		// handle something after fired
		if (!pDBConn->m_pDBConnPool->PostFireEvent(pEvent))
		{
			iLastEventSequenceId = 0;
		}
	}

	return 0;
}