/*****************************************************************************************
	filename:	cache_db_event.h
	created:	10/06/2012
	author:		chen
	purpose:	db event on cache server

*****************************************************************************************/

#ifndef _H_CACHEDB_EVENT
#define _H_CACHEDB_EVENT

#include "db_event.h"
#include "ftd_define.h"

enum
{
	DB_EVENT_GETAVATARLIST = 1,
	DB_EVENT_AVATARCREATE,
	DB_EVENT_AVATARSELECTDATA,
};

class PlayerDBEvent : public DBEvent
{
public:
	PlayerDBEvent()
	{
		m_iSessionId = 0;
		m_iRet = 0;
	}

public:
	uint32 m_iSessionId;
	int32 m_iRet;
};

class PlayerDBEventGetAvatarList : public PlayerDBEvent
{
public:
	PlayerDBEventGetAvatarList();
	int32 FireEvent(DBConn*);

public:
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
	uint8 m_iAvatarCount;
	prdAvatar m_arrayAvatar[AVATARCOUNT_MAX];
};

class PlayerDBEventAvatarCreate : public PlayerDBEvent
{
public:
	PlayerDBEventAvatarCreate();
	int32 FireEvent(DBConn*);

public:
	TCHAR m_strAccountName[ACCOUNTNAME_MAX+1];
	prdAvatar m_Avatar;
};

class PlayerDBEventAvatarSelectData : public PlayerDBEvent
{
public:
	PlayerDBEventAvatarSelectData();
	int32 FireEvent(DBConn*);

public:
	uint64 m_iAvatarId;
	TCHAR m_strAvatarName[AVATARNAME_MAX+1];
	uint8 m_iLastChannelId;
};

#endif
