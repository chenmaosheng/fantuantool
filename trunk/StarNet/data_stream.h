#ifndef _H_DATA_STREAM
#define _H_DATA_STREAM

#include "..\tool\common.h"

template<size_t DataLength = 0>
class DataStream
{
public:
	const static size_t sDataLength = DataLength;

	DataStream() : m_iDataLength(0), m_iDataIndex(0)
	{
		m_DataBuffer = new char[sDataLength];
	}

	DataStream(uint32 iLength, char* pBuf) : m_iDataLength(iLength), m_iDataIndex(0), m_DataBuffer(pBuf)
	{

	}

	~DataStream()
	{
		if (m_iDataIndex)
		{
			SAFE_DELETE(m_DataBuffer);
		}
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
	bool SerializeTo(const T& value)
	{
		if (m_iDataLength + sizeof(T) <= sDataLength)
		{
			memcpy(m_DataBuffer+m_iDataLength, &value, sizeof(T));
			m_iDataLength += sizeof(T);
			return true;
		}

		return false;
	}

	template<typename T>
	bool SerializeTo(uint16 iCount, const T* array)
	{
		iCount *= sizeof(T);
		if (m_iDataLength + iCount <= sDataLength)
		{
			memcpy(m_DataBuffer + m_iDataLength, array, iCount);
			m_iDataLength += iCount;
			return true;
		}

		return false;
	}

	template<typename T>
	bool SerializeFrom(T& value)
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
	bool SerializeFrom(uint16 iCount, T* array)
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
