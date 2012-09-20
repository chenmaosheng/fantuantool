#ifndef _H_DATA_STREAM
#define _H_DATA_STREAM

#include "common.h"

template<size_t DataLength>
class _InputStream
{
public:
	_InputStream() : m_iDataLength(0)
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
	char m_DataBuffer[DataLength];
};

typedef _InputStream<MAX_INPUT_BUFFER> InputStream;
typedef _InputStream<MAX_OUTPUT_BUFFER> PeerInputStream;

class OutputStream
{
public:
	OutputStream(uint32 iLength, char* pBuf) : m_iDataLength(iLength), m_iDataIndex(0), m_DataBuffer(pBuf)
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
	char* m_DataBuffer;
};

#endif
