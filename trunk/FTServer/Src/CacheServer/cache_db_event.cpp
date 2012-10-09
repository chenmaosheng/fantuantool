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
	memset(m_arrayAvatar, 0, sizeof(m_arrayAvatar));
}

int32 PlayerDBEventGetAvatarList::FireEvent(DBConn* pDBConn)
{
	int32 iRet = 0;
	TCHAR strSqlStatement[SQL_STATEMENT_MAX] = {0};
	swprintf_s(strSqlStatement, SQL_STATEMENT_MAX, _T("call SP_GetAvatarList('%s');"), m_strAccountName);

	m_iRet = pDBConn->Query(strSqlStatement);
	if (m_iRet != 0)
	{
		LOG_ERR(LOG_DB, _T("acc=%s sid=%08x ret=%d mysql query failed"), m_strAccountName, m_iSessionId, m_iRet);
		return m_iRet;
	}

	char** ppResult = NULL;
	int32 iNumOfRows = pDBConn->GetNumOfRows();
	// get data of each row
	for (int32 i = 0; i < iNumOfRows; ++i)
	{
		ppResult = pDBConn->GetRowData();
		m_arrayAvatar[m_iAvatarCount].m_iAvatarId = _atoi64(ppResult[0]);
		m_arrayAvatar[m_iAvatarCount].m_iAccountId = _atoi64(ppResult[1]);

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

	return 0;
}
