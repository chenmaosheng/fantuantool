/*****************************************************************************************
	filename:	avatar.h
	created:	10/18/2012
	author:		chen
	purpose:	record avatar info

*****************************************************************************************/

#ifndef _H_AVATAR
#define _H_AVATAR

#include "client_common.h"

class Avatar
{
public:
	uint64 m_iAvatarId;
	TCHAR m_strAvatarName[AVATARNAME_MAX+1];
};

#endif
