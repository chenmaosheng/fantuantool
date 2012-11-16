#include "db_conn_pool.h"
#include "db_conn.h"
#include "db_event.h"

DBConnPool::DBConnPool()
{
	m_strDBName[0] = '\0';
	m_strHost[0] = '\0';
	m_iPort = 0;
	m_strUserName[0] = '\0';
	m_strPassword[0] = '\0';

	m_hWaitingList = NULL;
}

int32 DBConnPool::Init(const char *strDBName, const char *strHost, uint16 iPort, const char *strUserName, const char *strPassword, uint16 iDBConnCount)
{
	DBConn* pDBConn = NULL;

	LOG_STT(LOG_DB, _T("Initialize DBPool, count=%d"), iDBConnCount);

	strcpy_s(m_strDBName, MAX_PATH, strDBName);
	strcpy_s(m_strHost, MAX_PATH, strHost);
	m_iPort = iPort;
	strcpy_s(m_strUserName, MAX_PATH, strUserName);
	strcpy_s(m_strPassword, MAX_PATH, strPassword);

	InitializeCriticalSection(&m_csEventWaitingList);
	m_hWaitingList = CreateEvent(NULL, TRUE, FALSE, NULL);
	InitializeCriticalSection(&m_csEventProcessingList);
	InitializeCriticalSection(&m_csEventReturnList);

	for (uint16 i = 0; i < iDBConnCount; ++i)
	{
		pDBConn = new DBConn;
		m_DBConnList.push_back(pDBConn);
		pDBConn->Init(this);
	}

	return 0;
}

void DBConnPool::Destroy()
{
	for (std::list<DBConn*>::iterator it = m_DBConnList.begin(); it != m_DBConnList.end(); ++it)
	{
		(*it)->Destroy();
		SAFE_DELETE(*it);
	}
	m_DBConnList.clear();

	if (!m_EventProcessingList.empty())
	{
		for (std::list<DBEvent*>::iterator it = m_EventProcessingList.begin(); it != m_EventProcessingList.end(); ++it)
		{
			FreeEvent(*it);
		}
		m_EventProcessingList.clear();
	}

	if (!m_EventWaitingList.empty())
	{
		for (std::list<DBEvent*>::iterator it = m_EventWaitingList.begin(); it != m_EventWaitingList.end(); ++it)
		{
			FreeEvent(*it);
		}
		m_EventWaitingList.clear();
	}

	if (!m_EventReturnList.empty())
	{
		for (std::list<DBEvent*>::iterator it = m_EventReturnList.begin(); it != m_EventReturnList.end(); ++it)
		{
			FreeEvent(*it);
		}

		m_EventReturnList.clear();
	}

	m_mEventProcessingSequenceMap.clear();

	DeleteCriticalSection(&m_csEventReturnList);
	DeleteCriticalSection(&m_csEventProcessingList);
	CloseHandle(m_hWaitingList);
	DeleteCriticalSection(&m_csEventWaitingList);

	LOG_STT(LOG_DB, _T("Destroy DBPool"));
}

void DBConnPool::Start()
{
	LOG_STT(LOG_DB, _T("Start DBPool"));

	for (std::list<DBConn*>::iterator it = m_DBConnList.begin(); it != m_DBConnList.end(); ++it)
	{
		(*it)->Start();
	}
}

void DBConnPool::Stop()
{
	for (std::list<DBConn*>::iterator it = m_DBConnList.begin(); it != m_DBConnList.end(); ++it)
	{
		(*it)->Stop();
	}

	for (std::list<DBConn*>::iterator it = m_DBConnList.begin(); it != m_DBConnList.end(); ++it)
	{
		(*it)->Join();
	}

	LOG_STT(LOG_DB, _T("Stop DBPool"));
}

int32 DBConnPool::PushSequenceEvent(uint64 iSequenceId, DBEvent* pEvent, bool bReturnable)
{
	if (iSequenceId == 0)
	{
		return -1;
	}

	pEvent->m_iSequenceId = iSequenceId;
	pEvent->m_bSequence = true;
	pEvent->m_bReturnable = bReturnable;
	
	EnterCriticalSection(&m_csEventWaitingList);
	m_EventWaitingList.push_back(pEvent);
	LeaveCriticalSection(&m_csEventWaitingList);

	// activate event
	PulseEvent(m_hWaitingList);

	return 0;
}

DBEvent* DBConnPool::PopFromDBEventReturnList()
{
	DBEvent* pEvent = NULL;

	EnterCriticalSection(&m_csEventReturnList);
	if (!m_EventReturnList.empty())
	{
		pEvent = m_EventReturnList.front();
		m_EventReturnList.pop_front();
	}
	LeaveCriticalSection(&m_csEventReturnList);

	return pEvent;
}

DBEvent* DBConnPool::PopFromDBEventProcessingList(uint64 iLastEventSequenceId)
{
	DBEvent* pEvent = NULL;

	EnterCriticalSection(&m_csEventProcessingList);

	// check if last event sequence id exists
	if (iLastEventSequenceId != 0)
	{
		stdext::hash_map<uint64, DBEvent*>::iterator mit = m_mEventProcessingSequenceMap.find(iLastEventSequenceId);
		if (mit != m_mEventProcessingSequenceMap.end())
		{
			pEvent = mit->second;
		}
	}

	if (!pEvent)
	{
		// check if processing list has nothing
		if (m_EventProcessingList.empty())
		{
			// get event from waiting list
			if (!_FlushEventWaitingList())
			{
				LeaveCriticalSection(&m_csEventProcessingList);
				WaitForSingleObject(m_hWaitingList, 1000);
				EnterCriticalSection(&m_csEventProcessingList);
			}
		}

		if (!m_EventProcessingList.empty())
		{
			pEvent = m_EventProcessingList.front();
			m_EventProcessingList.pop_front();
		}
	}

	LeaveCriticalSection(&m_csEventProcessingList);

	return pEvent;
}

bool DBConnPool::PostFireEvent(DBEvent* pEvent)
{
	bool bRet = false;
	
	EnterCriticalSection(&m_csEventProcessingList);
	// check if sequenced
	if (pEvent->m_bSequence)
	{
		stdext::hash_map<uint64, DBEvent*>::iterator mit = m_mEventProcessingSequenceMap.find(pEvent->m_iEventId);
		if (mit != m_mEventProcessingSequenceMap.end())
		{
			bRet = true;
		}

		if (bRet)
		{
			// check if event has next event
			if (pEvent->m_pNextEvent)
			{
				pEvent->m_pNextEvent->m_pLastEvent = NULL;
				m_mEventProcessingSequenceMap.insert(std::make_pair(pEvent->m_iEventId, pEvent->m_pNextEvent));
			}
			else
			{
				m_mEventProcessingSequenceMap.erase(mit);
			}

			pEvent->m_pLastEvent = NULL;
			pEvent->m_pNextEvent = NULL;
		}
	}
	LeaveCriticalSection(&m_csEventProcessingList);

	if (bRet)
	{
		if (pEvent->m_bReturnable)
		{
			_PushEventToReturnList(pEvent);
		}
		else
		{
			FreeEvent(pEvent);
		}
	}

	return bRet;
}

void DBConnPool::FreeEvent(DBEvent* pEvent)
{
	FT_DELETE(pEvent);
}

void DBConnPool::_PushEventToReturnList(DBEvent* pEvent)
{
	EnterCriticalSection(&m_csEventReturnList);
	m_EventReturnList.push_back(pEvent);
	LeaveCriticalSection(&m_csEventReturnList);
}

bool DBConnPool::_FlushEventWaitingList()
{
	DBEvent* pEvent = NULL;
	DBEvent* pLoopEvent = NULL;
	bool bRet = false;

	EnterCriticalSection(&m_csEventWaitingList);

	if (!m_EventWaitingList.empty())
	{
		for (std::list<DBEvent*>::iterator it = m_EventWaitingList.begin(); it != m_EventWaitingList.end(); ++it)
		{
			pEvent = (*it);
			if (pEvent->m_bSequence)
			{
				// check if exists in sequenced map
				stdext::hash_map<uint64, DBEvent*>::iterator mit = m_mEventProcessingSequenceMap.find(pEvent->m_iEventId);
				if (mit == m_mEventProcessingSequenceMap.end())
				{
					pEvent->m_pLastEvent = NULL;
					pEvent->m_pNextEvent = NULL;
					m_mEventProcessingSequenceMap.insert(std::make_pair(pEvent->m_iEventId, pEvent));
					// the list head must be put into the processing list
					m_EventProcessingList.push_back(pEvent);
				}
				else
				{
					// find the link end
					pLoopEvent = mit->second;
					while (pLoopEvent->m_pNextEvent)
					{
						pLoopEvent = pLoopEvent->m_pNextEvent;
					}
					pLoopEvent->m_pNextEvent = pEvent;
					pEvent->m_pLastEvent = pLoopEvent;
					pEvent->m_pNextEvent = NULL;
				}
			}
			else
			{
				m_EventProcessingList.push_back(pEvent);
			}
		}

		m_EventWaitingList.clear();
		bRet = true;
	}
	LeaveCriticalSection(&m_csEventWaitingList);

	return bRet;
}

