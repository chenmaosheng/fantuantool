// ClientSocket.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ClientSocket.h"
#include "ClientDlg.h"
#include "targetver.h"
#include "Command.h"
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
	char buf[1024] = {0};
	char* p = buf;
	Receive(buf, sizeof(buf));
	while ((*p) != 0)
	{
		Header* header = (Header*)p;
		if (header->type == LOGIN)
		{
			LoginPkt* pkt = (LoginPkt*)header;
			chatDlg->UpdateUser(pkt->nickname, pkt->connID, pkt->len-sizeof(pkt->connID));
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
			chatDlg->DeleteUser(pkt->connID);
		}

		p = p + (header->len + sizeof(Header));
	}
	
	CSocket::OnReceive(nErrorCode);
}
