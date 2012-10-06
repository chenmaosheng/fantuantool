/*****************************************************************************************
	filename:	db_conn_pool.h
	created:	10/06/2012
	author:		chen
	purpose:	db connector's pool

*****************************************************************************************/

#ifndef _H_DBCONN_POOL
#define _H_DBCONN_POOL

#include "server_common.h"
#include "memory_object.h"
#include <list>
#include <hash_map>

class DBConn;
class DBEvent;
class DBConnPool
{
public:
	// cstr
	DBConnPool();

	// initialize the pool
	int32 Init(const char* strDBName, const char* strHost, uint16 iPort, const char* strUserName, const char* strPassword, uint16 iDBConnCount);

	// destroy the pool
	void Destroy();

	// start the pool
	void Start();

	// stop the pool
	void Stop();

	// push a event to queue by sequence id
	int32 PushSequenceEvent(uint64 iSequenceId, DBEvent*, bool bReturnable = true);

	// pop a event from return event list
	DBEvent* PopFromDBEventReturnList();

	// pop a event from processing event list
	DBEvent* PopFromDBEventProcessingList(uint64 iLastEventSequenceId = 0);

	// do something after event fired
	bool PostFireEvent(DBEvent*);

	template<typename T>
	__inline T* AllocateEvent()
	{
		return FT_NEW(T);
	}

	void FreeEvent(DBEvent*);

private:
	// push a finished event to return list
	void _PushEventToReturnList(DBEvent*);

	// flush waiting list
	bool _FlushEventWaitingList();

private:
	// all the db connection
	std::list<DBConn*> m_DBConnList;
	// wait list of db event
	std::list<DBEvent*> m_EventWaitingList;
	CRITICAL_SECTION m_csEventWaitingList;
	HANDLE m_hWaitingList;

	// processing list of db event
	std::list<DBEvent*> m_EventProcessingList;
	CRITICAL_SECTION m_csEventProcessingList;
	stdext::hash_map<uint64, DBEvent*> m_mEventProcessingSequenceMap;

	// return value list of db event
	std::list<DBEvent*> m_EventReturnList;
	CRITICAL_SECTION m_csEventReturnList;

public:
	char m_strDBName[MAX_PATH];
	char m_strHost[MAX_PATH];
	uint16 m_iPort;
	char m_strUserName[MAX_PATH];
	char m_strPassword[MAX_PATH];
};

#endif
