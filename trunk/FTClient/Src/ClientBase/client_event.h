/*****************************************************************************************
	filename:	client_event.h
	created:	10/11/2012
	author:		chen
	purpose:	define event on client, served for UI

*****************************************************************************************/

#ifndef _H_CLIENT_EVENT
#define _H_CLIENT_EVENT

#include "client_common.h"
#include "memory_object.h"

enum
{
	EVENT_AVATAR_LIST,
	EVENT_AVATAR_LOGOUT,
};

struct ClientEvent : public MemoryObject
{
	int32 m_iEventId;
};

struct ClientEventAvatarList : public ClientEvent
{
	ClientEventAvatarList() 
	{ 
		m_iEventId = EVENT_AVATAR_LIST;
		m_iAvatarCount = 0;
		memset(m_Avatar, 0, sizeof(m_Avatar));
	}

	int32 m_iRet;
	uint8 m_iAvatarCount;
	ftdAvatar m_Avatar[AVATARCOUNT_MAX];
};

struct ClientEventAvatarLogout: public ClientEvent
{
	ClientEventAvatarLogout()
	{
		m_iEventId = EVENT_AVATAR_LOGOUT;
	}
};

#endif
