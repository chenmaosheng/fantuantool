#include "alarm.h"

Alarm::Alarm()
{
	m_dwCurrTime = 0;
}

Alarm::~Alarm()
{

}

void Alarm::Start(DWORD dwCurrTime)
{
	if (m_dwCurrTime == 0)
	{
		m_dwCurrTime = dwCurrTime;
	}
}

void Alarm::Tick(DWORD dwCurrTime)
{
	if (m_dwCurrTime == 0)
	{
		m_dwCurrTime = dwCurrTime;
		return;
	}

	m_dwCurrTime = dwCurrTime;

	for (std::list<AlarmEvent>::iterator it = m_AlarmEventList.begin(); it != m_AlarmEventList.end(); ++it)
	{
		AlarmEvent& event = (*it);
		if (event.m_dwStartTime + event.m_dwInterval >= m_dwCurrTime)
		{
			event.m_dwStartTime += (m_dwCurrTime - event.m_dwStartTime);
			(*(event.m_pListener))();
		}
	}
}
