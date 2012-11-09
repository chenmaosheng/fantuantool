#include "ring_buffer.h"

bool RingBuffer::Push(const char* pItem, size_t iLength)
{
	if (m_pTail > m_pHead)
	{
		size_t iRemainSize = m_iSize - (m_pTail - m_pHead);
		if (iRemainSize < iLength)
		{
			return false;
		}

		iRemainSize = m_pDataEnd - m_pTail;
		if (iRemainSize >= iLength)
		{
			memcpy(m_pTail, pItem, iLength);
			m_pTail += iLength;
		}
		else
		{
			memcpy(m_pTail, pItem, iRemainSize);
			memcpy(m_pData, pItem+iRemainSize, iLength - iRemainSize);
			m_pTail = m_pData + (iLength - iRemainSize);
		}
	}
	else
	{
		size_t iRemainSize = m_pHead - m_pTail;
		if (iRemainSize < iLength)
		{
			return false;
		}

		memcpy(m_pTail, pItem, iLength);
		m_pTail += iLength;
	}

	return true;
}

size_t RingBuffer::Pop(char* pItem, size_t iLength)
{
	if (m_pTail > m_pHead)
	{
		size_t iCurrSize = m_pTail - m_pHead;
		if (iCurrSize < iLength)
		{
			iLength = iCurrSize;
		}

		memcpy(pItem, m_pHead, iLength);
		m_pHead += iLength;
		return iLength;
	}
	else
	{
		size_t iCurrSize = m_pDataEnd - m_pHead;
		if (iCurrSize >= iLength)
		{
			memcpy(pItem, m_pHead, iLength);
			m_pHead += iLength;
			return iLength;
		}
		else
		{
			memcpy(pItem, m_pHead, iCurrSize);
			size_t iRemainSize = m_pTail - m_pData;
			if (iRemainSize > iLength - iCurrSize)
			{
				iRemainSize = iLength - iCurrSize;
			}
			memcpy(pItem, m_pData, iRemainSize);
			m_pHead = m_pData + iRemainSize;

			return iCurrSize + iRemainSize;
		}
	}
}