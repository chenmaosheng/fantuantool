#include "cache_db_event.h"
#include "db_conn.h"
#include "mysql.h"
#include <cstdio>

PlayerDBEventGetAvatarList::PlayerDBEventGetAvatarList()
{
	m_iEventId = DB_EVENT_GETAVATARLIST;
	m_strAccountName[0] = _T('\0');
	m_iAvatarCount = 0;
}

int32 PlayerDBEventGetAvatarList::FireEvent(DBConn* pDBConn)
{
	char strSqlStatement[1024] = {0};
	uint64 iAvatarId = 0;
	
	sprintf_s(strSqlStatement, 1024, "call SP_CreateAvatar('%s', '%s', %llu');",
		"chen", "chen", iAvatarId);
	int status = mysql_query(pDBConn->GetConnector(), strSqlStatement);
	if (status != 0)
	{
		return -1;
	}

	return 0;
}
