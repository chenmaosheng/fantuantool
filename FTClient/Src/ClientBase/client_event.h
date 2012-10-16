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
	EVENT_AVATAR_CREATE,
	EVENT_AVATAR_SELECT,
	EVENT_CHANNEL_LIST,
	EVENT_CHANNEL_SELECT,
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
		m_iReturn = 0;
		m_iAvatarCount = 0;
	}

	int32 m_iReturn;
	uint8 m_iAvatarCount;
	ftdAvatar m_Avatar[AVATARCOUNT_MAX];
};

struct ClientEventAvatarCreate: public ClientEvent
{
	ClientEventAvatarCreate()
	{
		m_iEventId = EVENT_AVATAR_CREATE;
		m_iReturn = 0;
	}

	int32 m_iReturn;
	ftdAvatar m_Avatar;
};

struct ClientEventAvatarSelect : public ClientEvent
{
	ClientEventAvatarSelect()
	{
		m_iEventId = EVENT_AVATAR_SELECT;
		m_iReturn = 0;
	}
	int32 m_iReturn;
	ftdAvatarSelectData m_SelectData;
};

struct ClientEventChannelList : public ClientEvent
{
	ClientEventChannelList()
	{
		m_iEventId = EVENT_CHANNEL_LIST;
		m_iReturn = 0;
		m_iChannelCount = 0;
	}

	int32 m_iReturn;
	ftdChannelData m_arrayChannelData[CHANNEL_MAX];
	uint8 m_iChannelCount;
};

struct ClientEventChannelSelect : public ClientEvent
{
	ClientEventChannelSelect()
	{
		m_iEventId = EVENT_CHANNEL_SELECT;
		m_iReturn = 0;
	}

	int32 m_iReturn;
};

struct ClientEventAvatarLogout: public ClientEvent
{
	ClientEventAvatarLogout()
	{
		m_iEventId = EVENT_AVATAR_LOGOUT;
	}
};

#endif
