/*****************************************************************************************
	filename:	cache_avatar_context.h
	created:	10/21/2012
	author:		chen
	purpose:	record avatar context on cache server

*****************************************************************************************/

#ifndef _H_CACHE_AVATAR_CONTEXT
#define _H_CACHE_AVATAR_CONTEXT

#include "server_common.h"

// all the data types of avatar
enum
{
	AVATAR_DATA_BASIC = 0,	// basic data
	AVATAR_DATA_LEVEL,	// avatar level

	AVATAR_DATA_MAX, 
};

struct CacheAvatarContext
{
	CacheAvatarContext();
	~CacheAvatarContext();
	void Clear();

	// basic data
	TCHAR m_strAvatarName[AVATARNAME_MAX+1];
	uint64 m_iAvatarId;

	// level data
	uint16 m_iLevel;

	bool m_arrayDirty[AVATAR_DATA_MAX];	// check which data is dirty
};

#endif
