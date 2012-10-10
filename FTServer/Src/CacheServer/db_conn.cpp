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
	m_pResult = NULL;
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

int32 DBConn::Query(const TCHAR* strStatement)
{
	char strSqlStatement[SQL_STATEMENT_MAX] = {0};
	int32 iRet = 0;
	TCHAR errCode[SQL_ERROR_CODE] = {0};

	FreeResult();

	iRet = WChar2Char(strStatement, strSqlStatement, SQL_STATEMENT_MAX);
	if (iRet == 0)
	{
		LOG_ERR(LOG_DB, _T("WChar2Char failed"));
		return -1;
	}

	strSqlStatement[iRet] = '\0';

	iRet = mysql_real_query(m_pMySQL, strSqlStatement, iRet+1);
	if (iRet != 0)
	{
		Char2WChar(mysql_error(m_pMySQL), errCode, SQL_ERROR_CODE);
		LOG_ERR(LOG_DB, _T("mysql query failed, err=%s"), errCode);
		return -2;
	}

	m_pResult = mysql_store_result(m_pMySQL);
	if (!m_pResult)
	{
		Char2WChar(mysql_error(m_pMySQL), errCode, SQL_ERROR_CODE);
		LOG_ERR(LOG_DB, _T("get result failed, err=%s"), errCode);
		return -3;
	}

	return 0;
}

int32 DBConn::GetNumOfRows()
{
	if (!m_pResult)
	{
		return 0;
	}

	return (int32)mysql_num_rows(m_pResult);
}

char** DBConn::GetRowData()
{
	if (!m_pResult)
	{
		return NULL;
	}

	return (char**)mysql_fetch_row(m_pResult);
}

void DBConn::FreeResult()
{
	if (m_pResult)
	{
		mysql_free_result(m_pResult);
	}

	// reminder: here is a new concept, you have to free all results in mysql
	// and otherwise you can't call another sql statement
	do 
	{
		m_pResult = mysql_store_result(m_pMySQL);
		mysql_free_result(m_pResult);
	} while (!mysql_next_result(m_pMySQL));
}

bool DBConn::_InitConnector()
{
	TCHAR errCode[SQL_ERROR_CODE] = {0};
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

		Char2WChar(mysql_error(m_pMySQL), errCode, SQL_ERROR_CODE);
		LOG_ERR(LOG_DB, _T("Connect to db failed, err=%s"), errCode);
		return false;
	}

	LOG_ERR(LOG_DB, _T("initialize db failed"));

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
			_ASSERT( false && _T("FireEvent failed") );
			LOG_ERR(LOG_DB, _T("FireEvent failed"));
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