/*****************************************************************************************
	filename:	data_stream.h
	created:	09/27/2012
	author:		chen
	purpose:	serialize into/from data, only for C/S

*****************************************************************************************/

#ifndef _H_DATA_STREAM
#define _H_DATA_STREAM

#include "starnet_common.h"

// stream for sending
template<size_t DataLength>
class _OutputStream
{
public:
	_OutputStream() : m_iDataLength(0)
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
	char m_DataBuffer[DataLength];
};

typedef _OutputStream<MAX_INPUT_BUFFER> OutputStream;
typedef _OutputStream<MAX_OUTPUT_BUFFER> ServerOutputStream;

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
