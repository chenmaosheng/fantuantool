/*****************************************************************************************
	filename:	ring_buffer.h
	created:	11/09/2012
	author:		chen
	purpose:	circular buffer

*****************************************************************************************/

#ifndef _H_RING_BUFFER
#define _H_RING_BUFFER

#include "common.h"

class RingBuffer
{
public:
	// Constructor
	RingBuffer(size_t max_size) : m_iSize(max_size)
	{
		m_pData = (char*)_aligned_malloc(max_size, MEMORY_ALLOCATION_ALIGNMENT);
		memset(m_pData, 0, max_size);
		m_pHead = m_pData;
		m_pTail = m_pData;
		m_pDataEnd = m_pData + max_size;
	}

	// destructor
	~RingBuffer()
	{
		_aligned_free(m_pData);
	}

	// write to data
	bool Push(const char* pItem, size_t iLength);

	// pop from data
	size_t Pop(char* pItem, size_t iLength);

private:
	char*			m_pData;		// Buffer
	char*			m_pDataEnd;		// end of the buffer;
	char* volatile	m_pHead;		// point to the head of the buffer
	char* volatile	m_pTail;		// point to the end of the buffer
	size_t			m_iSize;		// whole length of buffer
};

#endif
