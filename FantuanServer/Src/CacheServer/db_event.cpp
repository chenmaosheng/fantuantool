#include "db_event.h"

DBEvent::DBEvent()
{
	m_iEventId = 0;
	m_bSequence = false;
	m_bFired = false;
	m_iSequenceId = 0;
	m_bReturnable = false;

	m_pLastEvent = NULL;
	m_pNextEvent = NULL;
}

DBEvent::~DBEvent()
{
}