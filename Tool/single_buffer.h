#ifndef _H_SINGLE_BUFFER
#define _H_SINGLE_BUFFER

class SingleBuffer
{
public:
	// Constructor
	SingleBuffer(size_t max_size) : m_iSize(max_size), m_iCurrSize(0)
	{
		// 分配一块对齐的内存
		m_pData = _aligned_malloc(max_size, MEMORY_ALLOCATION_ALIGNMENT);
		memset(m_pData, 0, max_size);
		m_pTail = m_pData;
	}

	// Destructor
	~SingleBuffer()
	{
		_aligned_free(m_pData);
	}

	// 往Buffer中写入一定长度的数据
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

	// 从Buffer中读出一定长度的数据
	bool	Pop(void* pItem, size_t iLength)
	{
		if (pItem == NULL)
		{
			return false;
		}

		if (m_iCurrSize < iLength)
		{
			m_iCurrSize = iLength;
		}

		m_pTail = (BYTE*)m_pTail - iLength;
		memcpy(pItem, m_pTail, iLength);

		m_iCurrSize -= iLength;
		return true;
	}

	// get current size
	size_t	GetCurrSize()	const
	{
		return m_iCurrSize;
	}

	// get the last length of buffer
	size_t	GetLastSize()	const
	{
		return m_iSize - m_iCurrSize;
	}

private:
	void*			m_pData;		// Buffer的数据
	void* volatile	m_pTail;		// 指向写入的数据尾部
	size_t			m_iSize;		// Buffer的总大小
	size_t			m_iCurrSize;	// 使用的大小
};

#endif