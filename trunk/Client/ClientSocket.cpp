// ClientSocket.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientSocket.h"
#include "ClientDlg.h"
#include "targetver.h"
#include "Command.h"
#include "data_stream.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientSocket

CClientSocket::CClientSocket()
{
	m_iRecvBufLen = 0;
	m_RecvBuf[0] = '\0';
	m_iState = NOT_CONNECT;
}

CClientSocket::~CClientSocket()
{
}


// Do not edit the following lines, which are needed by ClassWizard.
#if 0
BEGIN_MESSAGE_MAP(CClientSocket, CSocket)
	//{{AFX_MSG_MAP(CClientSocket)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
#endif	// 0

/////////////////////////////////////////////////////////////////////////////
// CClientSocket member functions

void CClientSocket::OnReceive(int nErrorCode) 
{
	char buf[65536] = {0};
	char* pBuf = buf;
	int iLen = Receive(buf, sizeof(buf));

	uint32 iCopyLen = 0;
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

		if (m_iState == CONNECTED)
		{
			m_iState = LOGGEDIN;

			VersionReq(NULL, 1);
		}
		else
		{
			while (m_iRecvBufLen > SERVER_PACKET_HEAD)	// step2: check if buffer is larger than header
			{
				ServerPacket* pServerPacket = (ServerPacket*)m_RecvBuf;
				uint16 iFullLength = pServerPacket->m_iLen+SERVER_PACKET_HEAD;
				if (m_iRecvBufLen >= iFullLength)	// step3: cut specific size from received buffer
				{
					iRet = HandlePacket(pServerPacket);
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
		}
		
	}while (iLen);
	
	CSocket::OnReceive(nErrorCode);
}

int32 CClientSocket::HandlePacket(ServerPacket* pPacket)
{
	return chatDlg->HandlePacket(pPacket);
}

int32 CClientSocket::VersionReq(void* pClient, int32 iVersion)
{
	OutputStream stream;
	stream.Serialize(iVersion);	// step2: serialize parameters to a datastream		

	SendPacket(pClient, (CLIENT_FILTER_LOGIN<<8)|0, stream.GetDataLength(), stream.GetBuffer());		// step3: retrieve buf and len from datastream

	return 0;
}

int32 CClientSocket::SendPacket(void* pClient, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	char outBuf[1024] = {0};
	ServerPacket* pServerPacket = (ServerPacket*)outBuf;
	pServerPacket->m_iLen = iLen;
	pServerPacket->m_iTypeId = iTypeId;
	memcpy(pServerPacket->m_Buf, pBuf, iLen);		// step4: use packet to wrap this buf and add header info

	Send(outBuf,iLen+SERVER_PACKET_HEAD); // step5: send it

	return 0;
}