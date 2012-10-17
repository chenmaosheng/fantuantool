/*****************************************************************************************
	filename:	auto_locker.h
	created:	10/17/2012
	author:		chen
	purpose:	thread lock and unlock automatically

*****************************************************************************************/

#ifndef _H_AUTO_LOCKER
#define _H_AUTO_LOCKER

#include "common.h"

// todo: in the future, use srw locker instead
class AutoLocker
{
public:
	AutoLocker(LPCRITICAL_SECTION pCS) : m_pCS(pCS)
	{
		EnterCriticalSection(m_pCS);
	}

	~AutoLocker()
	{
		LeaveCriticalSection(m_pCS);
	}

private:
	LPCRITICAL_SECTION m_pCS;
};

#endif
