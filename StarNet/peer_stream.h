#ifndef _H_PEER_STREAM
#define _H_PEER_STREAM

#include "peer_packet.h"

class PeerOutputStream
{
public:
	PeerOutputStream();
	~PeerOutputStream();

	int32 Send(PEER_CLIENT);
	int32 Send(PEER_SERVER);

	void SetId(uint16 iFilterId, uint16 iFuncId)
	{
		m_pPacket->m_iFilterId = iFilterId;
		m_pPacket->m_iFuncId = iFuncId;
	}

	template<typename T>
	bool Serialize(const T& value)
	{
		if (m_pPacket->m_iLen + sizeof(T) <= MAX_PEER_BUFFER)
		{
			memcpy(m_pPacket->m_Buf+m_pPacket->m_iLen, &value, sizeof(T));
			m_pPacket->m_iLen += sizeof(T);
			return true;
		}

		return false;
	}

	template<typename T>
	bool Serialize(uint16 iCount, const T* array)
	{
		iCount *= sizeof(T);

		if (m_pPacket->m_iLen + iCount <= MAX_PEER_BUFFER)
		{
			memcpy(m_pPacket->m_Buf+m_pPacket->m_iLen, array, iCount);
			m_pPacket->m_iLen += iCount;
			return true;
		}

		return false;
	}

private:
	PeerPacket* m_pPacket;
};

class PeerInputStream
{
public:
	PeerInputStream(uint32 iLength, const char* pBuf) : m_iDataLength(iLength), m_iDataIndex(0), m_DataBuffer(pBuf)
	{
	}

	uint32 GetDataLength() const
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
	bool Serialize(uint16 iCount, T*& array)
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
	uint32 m_iDataLength;
	uint32 m_iDataIndex;
	const char* m_DataBuffer;
};

#endif
