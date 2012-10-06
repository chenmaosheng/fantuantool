/*****************************************************************************************
	filename:	db_event.h
	created:	10/06/2012
	author:		chen
	purpose:	define db event base class

*****************************************************************************************/

#ifndef _H_DB_EVENT
#define _H_DB_EVENT

#include "server_common.h"
#include "memory_object.h"

class DBConn;
class DBEvent : public MemoryObject
{
public:
	DBEvent();
	virtual ~DBEvent();

	// execute the db event done by db conn
	virtual int32 FireEvent(DBConn*) = 0;

public:
	uint32 m_iEventId;	// id of db event
	bool m_bFired;	// check if it's fired
	uint64 m_iSequenceId; // id of event sequence, defined by user
	bool m_bSequence;	// need to fire by sequence
	bool m_bReturnable;

	DBEvent* m_pLastEvent;
	DBEvent* m_pNextEvent;
};

#endif
