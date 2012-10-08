/*****************************************************************************************
	filename:	alarm.h
	created:	10/08/2012
	author:		chen
	purpose:	create an alarm, when the alarm is activated, it will call a function

*****************************************************************************************/

#ifndef _H_ALARM
#define _H_ALARM

#include "common.h"
#include "listener.h"
#include <list>

// todo: consider some events only need to call once
struct AlarmEvent
{
	TCHAR m_strEventName[MAX_PATH];	// event name as identifier
	DWORD m_dwStartTime; // start point each time, if event is called, then reset
	DWORD m_dwInterval; // interval of each call
	ListenerBase* m_pListener; // call functor

	AlarmEvent(const TCHAR* strName, DWORD dwStartTime, DWORD dwInterval, ListenerBase* pListener)
	{
		wcscpy_s(m_strEventName, _countof(m_strEventName), strName);
		m_dwStartTime = dwStartTime;
		m_dwInterval = dwInterval;
		m_pListener = pListener;
	}
};

class Alarm
{
public:
	// cstr and dstr
	Alarm();
	~Alarm();

	void Start(DWORD dwCurrTime);
	void Tick(DWORD dwCurrTime);

	template<typename Class>
	void RegisterEvent(const TCHAR* strName, DWORD dwStartTime, DWORD dwInterval, Class* pClass, void (Class::*Func)())
	{
		m_AlarmEventList.push_back(AlarmEvent(strName, dwStartTime, dwInterval, new Listener<>(Func, pClass)));
	}

	template<typename Class, typename Arg1>
	void RegisterEvent(const TCHAR* strName, DWORD dwStartTime, DWORD dwInterval, Class* pClass, void (Class::*Func)(Arg1), Arg1 a1)
	{
		m_AlarmEventList.push_back(AlarmEvent(strName, dwStartTime, dwInterval, new Listener<TYPELIST_1(Arg1)>(Func, pClass, a1)));
	}

	template<typename Class, typename Arg1, typename Arg2>
	void RegisterEvent(const TCHAR* strName, DWORD dwStartTime, DWORD dwInterval, Class* pClass, void (Class::*Func)(Arg1, Arg2), Arg1 a1, Arg2 a2)
	{
		m_AlarmEventList.push_back(AlarmEvent(strName, dwStartTime, dwInterval, new Listener<TYPELIST_2(Arg1, Arg2)>(Func, pClass, a1, a2)));
	}

	template<typename Class, typename Arg1, typename Arg2, typename Arg3>
	void RegisterEvent(const TCHAR* strName, DWORD dwStartTime, DWORD dwInterval, Class* pClass, void (Class::*Func)(Arg1, Arg2, Arg3), Arg1 a1, Arg2 a2, Arg3 a3)
	{
		m_AlarmEventList.push_back(AlarmEvent(strName, dwStartTime, dwInterval, new Listener<TYPELIST_3(Arg1, Arg2, Arg3)>(Func, pClass, a1, a2, a3)));
	}

private:
	DWORD m_dwCurrTime;
	std::list<AlarmEvent> m_AlarmEventList;
};
#endif
