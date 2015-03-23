#include "session.h"
#include "server_base.h"
#include "connection.h"
#include "packet.h"
#include "logic_command.h"
#include "logic_loop.h"
#include "context.h"
#include "dispatcher.h"

uint32 Session::m_iSessionIdStart = 0;
ServerBase* Session::m_pServer = NULL;

int32 Session::Initialize(ServerBase* pServer)
{
	m_pServer = pServer;
	return 0;
}

Session::Session()
{
	m_iSessionId = 0;
	m_pConnection = NULL;
	m_iRecvBufLen = 0;
	m_RecvBuf[0] = '\0';
	m_dwConnectionTime = 0;
	m_dwLoggedInTime = 0;
}

Session::~Session()
{
	m_pConnection = NULL;
}

void Session::Clear()
{
	m_pConnection = NULL;
	m_iRecvBufLen = 0;
	m_RecvBuf[0] = '\0';
	m_dwConnectionTime = 0;
	m_dwLoggedInTime = 0;
}

int32 Session::OnConnection(ConnID connId)
{
	int32 iRet = 0;

	m_iSessionId = ++m_iSessionIdStart;

	// coordinate conn and session
	m_pConnection = (Connection*)connId;
	m_pConnection->SetClient(this);
	// set session's onconnection time
	m_dwConnectionTime = m_pServer->GetCurrTime();
	return 0;
}

void Session::OnDisconnect()
{
	if (m_pConnection)
	{
		Connection::Close(m_pConnection);
		m_pConnection = NULL;
	}

	Clear();
}

void Session::OnData(uint16 iLen, char* pBuf)
{
	uint16 iCopyLen = 0;
	int32 iRet = 0;

	do
	{
		// the incoming length is no more than the last of buffer
		if (m_iRecvBufLen + iLen <= sizeof(m_RecvBuf))
		{
			memcpy(m_RecvBuf + m_iRecvBufLen, pBuf, iLen);
			m_iRecvBufLen += iLen;
			pBuf += iLen;
			iLen = 0;
		}
		else
		{
			iCopyLen = m_iRecvBufLen + iLen - sizeof(m_RecvBuf);
			memcpy(m_RecvBuf + m_iRecvBufLen, pBuf, iCopyLen);
			pBuf += iCopyLen;
			iLen -= iCopyLen;
			m_iRecvBufLen += iCopyLen;
		}	// step1: received a raw buffer

		while (m_iRecvBufLen >= PACKET_HEAD)	// step2: check if buffer is larger than header
		{
			Packet* pPacket = (Packet*)m_RecvBuf;
			uint16 iFullLength = pPacket->m_iLen+PACKET_HEAD;
			if (m_iRecvBufLen >= iFullLength)	// step3: cut specific size from received buffer
			{
				iRet = HandlePacket(pPacket);
				if (iRet != 0)
				{
					return;
				}
					
				if (m_iRecvBufLen > iFullLength)
				{
					memmove(m_RecvBuf, m_RecvBuf + iFullLength, m_iRecvBufLen - iFullLength);
				}
				m_iRecvBufLen -= iFullLength;
			}
			else
			{
				break;
			}
		}
	}while (iLen);
}

void Session::Disconnect()
{
	if (m_pConnection)
	{
		m_pConnection->AsyncDisconnect();
	}
	else
	{
		OnDisconnect();
	}
}

int32 Session::SendData(uint16 iTypeId, uint16 len, const char *data)
{
	Context* pContext = Context::CreateContext(OPERATION_SEND);
	Packet* pPacket = (Packet*)pContext->buffer_;
	pPacket->m_iLen = len;
	pPacket->m_iTypeId = iTypeId;

	memcpy(pPacket->m_Buf, data, len);
	m_pConnection->AsyncSend(pPacket->m_iLen + PACKET_HEAD, pContext->buffer_);

	return 0;
}

int32 Session::HandlePacket(Packet* pPacket)
{
	Receiver::OnPacketReceived(this, pPacket->m_iTypeId, pPacket->m_iLen, pPacket->m_Buf);
	return 0;
}
