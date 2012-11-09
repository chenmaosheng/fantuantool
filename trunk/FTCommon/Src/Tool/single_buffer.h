/*****************************************************************************************
	filename:	single_buffer.h
	created:	09/27/2012
	author:		chen
	purpose:	single-direction buffer

*****************************************************************************************/

#ifndef _H_SINGLE_BUFFER
#define _H_SINGLE_BUFFER

class SingleBuffer
{
public:
	// Constructor
	SingleBuffer(size_t max_size) : m_iSize(max_size), m_iCurrSize(0)
	{
		// allocate an aligned memory
		m_pData = _aligned_malloc(max_size, MEMORY_ALLOCATION_ALIGNMENT);
		memset(m_pData, 0, max_size);
		m_pTail = m_pData;
	}

	// Destructor
	~SingleBuffer()
	{
		_aligned_free(m_pData);
	}

	// write some data to buffer
	bool	Push(const void* pItem, size_t iLength)
	{
		if (m_iCurrSize + iLength >= m_iSize)
		{
			return false;
		}

		memcpy(m_pTail, pItem, iLength);

		m_pTail = (BYTE*)m_pTail + iLength;
		m_iCurrSize += iLength;
	
		return true;
	}

	// read some data from buffer
	size_t	Pop(void* pItem, size_t iLength)
	{
		if (pItem == NULL)
		{
			return 0;
		}

		if (m_iCurrSize < iLength)
		{
			iLength = m_iCurrSize;
		}

		m_pTail = (BYTE*)m_pTail - iLength;
		memcpy(pItem, m_pTail, iLength);

		m_iCurrSize -= iLength;
		return iLength;
	}

	// get current size
	size_t	GetCurrSize()	const
	{
		return m_iCurrSize;
	}

	// get the remain length of buffer
	size_t	GetRemainSize()	const
	{
		return m_iSize - m_iCurrSize;
	}

private:
	void*			m_pData;		// Buffer
	void* volatile	m_pTail;		// point to the end of the buffer
	size_t			m_iSize;		// whole length of buffer
	size_t			m_iCurrSize;	// currently used length of buffer
};

#endif