// ClientSocket.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientSocket.h"
#include "ClientDlg.h"
#include "targetver.h"
#include "Command.h"
#include "packet.h"
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
	// TODO: Add your specialized code here and/or call the base class
	char buf[65536] = {0};
	char* p = buf;
	Receive(buf, sizeof(buf));
	while ((*p) != 0)
	{
		ServerPacket* pPacket = (ServerPacket*)p;
		InputStream stream(pPacket->m_iLen, pPacket->m_Buf);
		if (pPacket->m_iTypeId == LOGIN_NTF)
		{
			uint32 iSessionId = 0;
			stream.Serialize(iSessionId);
			uint16 iLength = 0;
			stream.Serialize(iLength);
			char nickname[64] = {0};
			stream.Serialize(iLength, nickname);
			chatDlg->UpdateUser(nickname, iSessionId, iLength);
		}
		/*Header* header = (Header*)p;
		if (header->type == LOGIN)
		{
			LoginPkt* pkt = (LoginPkt*)header;
			chatDlg->UpdateUser(pkt->nickname, pkt->connId, pkt->len-sizeof(pkt->connId));
		}
		else
		if (header->type == SEND_MESSAGE)
		{
			SendMessagePkt* pkt = (SendMessagePkt*)header;
			chatDlg->GetMessage(pkt->message, pkt->len);
		}
		else
		if (header->type == LOGOUT)
		{
			LogoutPkt* pkt = (LogoutPkt*)header;
			chatDlg->DeleteUser(pkt->connId);
		}*/

		p = p + (pPacket->m_iLen + SERVER_PACKET_HEAD);
	}
	
	CSocket::OnReceive(nErrorCode);
}
