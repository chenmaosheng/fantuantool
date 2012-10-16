#include "cache_db_event.h"
#include "db_conn.h"

#include "ftd_define.h"

#include "mysql.h"
#include <cstdlib>

PlayerDBEventGetAvatarList::PlayerDBEventGetAvatarList()
{
	m_iEventId = DB_EVENT_GETAVATARLIST;
	m_strAccountName[0] = _T('\0');
	m_iAvatarCount = 0;
}

int32 PlayerDBEventGetAvatarList::FireEvent(DBConn* pDBConn)
{
	int32 iRet = 0;
	TCHAR strSqlStatement[SQL_STATEMENT_MAX] = {0};
	swprintf_s(strSqlStatement, SQL_STATEMENT_MAX, _T("call SP_GetAvatarList('%s');"), m_strAccountName);

	iRet = pDBConn->Query(strSqlStatement);
	if (iRet < 0)
	{
		LOG_ERR(LOG_DB, _T("acc=%s sid=%08x ret=%d mysql query failed"), m_strAccountName, m_iSessionId, m_iRet);
		return iRet;
	}

	if (iRet == 0)
	{
		char** ppResult = NULL;
		int32 iNumOfRows = pDBConn->GetNumOfRows();
		// get data of each row
		for (int32 i = 0; i < iNumOfRows; ++i)
		{
			ppResult = pDBConn->GetRowData();
			m_arrayAvatar[m_iAvatarCount].m_iAvatarId = _atoi64(ppResult[0]);
			iRet = Char2WChar(ppResult[2], m_arrayAvatar[m_iAvatarCount].m_strAvatarName, AVATARNAME_MAX+1);
			if (iRet == 0)
			{
				LOG_ERR(LOG_DB, _T("acc=%s sid=%08x Char2WChar failed"), m_strAccountName, m_iSessionId);
				m_iRet = -1;
				return m_iRet;
			}
			m_arrayAvatar[m_iAvatarCount].m_strAvatarName[iRet] = _T('\0');
			m_iAvatarCount++;
		}
	}

	// free all possible results
	pDBConn->FreeResult();

	m_iRet = 0;
	return 0;
}

PlayerDBEventAvatarCreate::PlayerDBEventAvatarCreate()
{
	m_iEventId = DB_EVENT_AVATARCREATE;
	m_strAccountName[0] = _T('\0');
}

int32 PlayerDBEventAvatarCreate::FireEvent(DBConn* pDBConn)
{
	int32 iRet = 0;
	TCHAR strSqlStatement[SQL_STATEMENT_MAX] = {0};
	swprintf_s(strSqlStatement, SQL_STATEMENT_MAX, _T("call SP_CreateAvatar('%s', '%s');"), m_strAccountName, m_Avatar.m_strAvatarName);
	LOG_DBG(LOG_DB, _T("acc=%s sid=%08x sql=%s"), m_strAccountName, m_iSessionId, strSqlStatement);

	iRet = pDBConn->Query(strSqlStatement);
	if (iRet < 0)
	{
		LOG_ERR(LOG_DB, _T("acc=%s sid=%08x ret=%d mysql query failed"), m_strAccountName, m_iSessionId, m_iRet);
		return iRet;
	}

	if (iRet == 0)
	{
		char** ppResult = NULL;
		int32 iNumOfRows = pDBConn->GetNumOfRows();
		_ASSERT( iNumOfRows == 1 );
		// get data of each row
		for (int32 i = 0; i < iNumOfRows; ++i)
		{
			ppResult = pDBConn->GetRowData();
			m_Avatar.m_iAvatarId = _atoi64(ppResult[0]);
		}
	}

	// free all possible results
	pDBConn->FreeResult();

	m_iRet = 0;
	return 0;
}

PlayerDBEventAvatarSelectData::PlayerDBEventAvatarSelectData()
{
	m_iEventId = DB_EVENT_AVATARSELECTDATA;
	m_iAvatarId = 0;
	m_strAvatarName[0] = _T('\0');
	m_iLastChannelId = 0;
}

int32 PlayerDBEventAvatarSelectData::FireEvent(DBConn* pDBConn)
{
	int32 iRet = 0;
	TCHAR strSqlStatement[SQL_STATEMENT_MAX] = {0};
	swprintf_s(strSqlStatement, SQL_STATEMENT_MAX, _T("call SP_SelectAvatar('%llu');"), m_iAvatarId);

	iRet = pDBConn->Query(strSqlStatement);
	if (iRet < 0)
	{
		LOG_ERR(LOG_DB, _T("name=%s sid=%08x ret=%d mysql query failed"), m_strAvatarName, m_iSessionId, m_iRet);
		return iRet;
	}

	if (iRet == 0)
	{
		char** ppResult = NULL;
		int32 iNumOfRows = pDBConn->GetNumOfRows();
		_ASSERT( iNumOfRows == 1 );
		// get data of each row
		for (int32 i = 0; i < iNumOfRows; ++i)
		{
			ppResult = pDBConn->GetRowData();
			m_iLastChannelId = (uint8)atoi(ppResult[0]);
		}
	}

	// free all possible results
	pDBConn->FreeResult();

	m_iRet = 0;
	return 0;
}

PlayerDBEventAvatarEnterRegion::PlayerDBEventAvatarEnterRegion()
{
	m_iEventId = DB_EVENT_AVATARENTERREGION;
	m_iAvatarId = 0;
	m_strAvatarName[0] = _T('\0');
}

int32 PlayerDBEventAvatarEnterRegion::FireEvent(DBConn* pDBConn)
{
	m_iRet = 0;
	return 0;
}
