#ifndef _H_EASY_STREAM
#define _H_EASY_STREAM

#include "common.h"

class Stream
{
public:
	Stream(uint32 iLength=0, char* pBuf=NULL) : m_iDataLen(iLength), m_iDataIndex(0), m_pDataBuffer(pBuf), m_bDelete(false)
	{
		if (pBuf == NULL)
		{
			m_pDataBuffer = (char*)_aligned_malloc(MAX_BUFFER, MEMORY_ALLOCATION_ALIGNMENT);
			m_bDelete = true;
		}
	}

	~Stream()
	{
		if (m_bDelete)
		{
			_aligned_free(m_pDataBuffer);
		}
	}

	uint16 GetDataLength() const
	{
		return m_iDataLen;
	}

	const char* GetBuffer() const
	{
		return m_pDataBuffer;
	}

	template<typename T>
	bool Unserialize(const T& value)
	{
		// serialize common type
		if (m_iDataLen + sizeof(T) <= sizeof(m_pDataBuffer))
		{
			memcpy(m_DataBuffer+m_iDataLen, &value, sizeof(T));
			m_iDataLen += sizeof(T);
			return true;
		}

		return false;
	}

	template<typename T>
	bool Unserialize(uint16 iCount, const T* array)
	{
		// serialize array type
		iCount *= sizeof(T);
		if (m_iDataLen + iCount <= sizeof(m_pDataBuffer))
		{
			memcpy(m_DataBuffer + m_iDataLen, array, iCount);
			m_iDataLen += iCount;
			return true;
		}

		return false;
	}

	template<typename T>
	bool Serialize(T& value)
	{
		// serialize common type
		if (m_iDataLen - m_iDataIndex >= sizeof(T))
		{
			memcpy(&value, m_pDataBuffer + m_iDataIndex, sizeof(T));
			m_iDataIndex += sizeof(T);
			return true;
		}

		return false;
	}

	template<typename T>
	bool Serialize(uint16 iCount, T* array)
	{
		// serialize array type
		iCount *= sizeof(T);
		if (m_iDataLen - m_iDataIndex >= iCount)
		{
			memcpy(array, m_pDataBuffer + m_iDataIndex, iCount);
			m_iDataIndex += iCount;
			return true;
		}

		return false;
	}

private:
	uint16 m_iDataLen;
	uint16 m_iDataIndex;
	char* m_pDataBuffer;
	bool m_bDelete;
};

// stream for sending
class OutputStream
{
public:
	OutputStream() : m_iDataLength(0)
	{
		memset(m_DataBuffer, 0, sizeof(m_DataBuffer));
	}

	uint16 GetDataLength() const
	{
		return m_iDataLength;
	}

	const char* GetBuffer() const
	{
		return m_DataBuffer;
	}

	template<typename T>
	bool Serialize(const T& value)
	{
		// serialize common type
		if (m_iDataLength + sizeof(T) <= sizeof(m_DataBuffer))
		{
			memcpy(m_DataBuffer+m_iDataLength, &value, sizeof(T));
			m_iDataLength += sizeof(T);
			return true;
		}

		return false;
	}

	template<typename T>
	bool Serialize(uint16 iCount, const T* array)
	{
		// serialize array type
		iCount *= sizeof(T);
		if (m_iDataLength + iCount <= sizeof(m_DataBuffer))
		{
			memcpy(m_DataBuffer + m_iDataLength, array, iCount);
			m_iDataLength += iCount;
			return true;
		}

		return false;
	}

private:
	uint16 m_iDataLength;
	char m_DataBuffer[MAX_BUFFER];
};

// stream for receiving
class InputStream
{
public:
	InputStream(uint32 iLength, const char* pBuf) : m_iDataLength(iLength), m_iDataIndex(0), m_DataBuffer(pBuf)
	{
	}

	uint16 GetDataLength() const
	{
		return m_iDataLength;
	}

	const char* GetBuffer() const
	{
		return m_DataBuffer;
	}

	template<typename T>
	bool Serialize(T& value)
	{
		// serialize common type
		if (m_iDataLength - m_iDataIndex >= sizeof(T))
		{
			memcpy(&value, m_DataBuffer + m_iDataIndex, sizeof(T));
			m_iDataIndex += sizeof(T);
			return true;
		}

		return false;
	}

	template<typename T>
	bool Serialize(uint16 iCount, T* array)
	{
		// serialize array type
		iCount *= sizeof(T);
		if (m_iDataLength - m_iDataIndex >= iCount)
		{
			memcpy(array, m_DataBuffer + m_iDataIndex, iCount);
			m_iDataIndex += iCount;
			return true;
		}

		return false;
	}

private:
	uint16 m_iDataLength;
	uint16 m_iDataIndex;
	const char* m_DataBuffer;
};

#endif
