// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "LoginDlg.h"
#include "ClientSocket.h"
#include "targetver.h"
#include "basic_packet.h"
#include "command.h"
#include "data_stream.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CClientApp theApp;
/////////////////////////////////////////////////////////////////////////////



// CLoginDlg dialog


CLoginDlg::CLoginDlg(CClientSocket *p_Socket, CWnd* pParent /*=NULL*/)
: CDialog(CLoginDlg::IDD, pParent)

{
	ASSERT(p_Socket);
	m_pSocket = p_Socket;
	//{{AFX_DATA_INIT(CLoginDlg)
	m_strName = _T("Your name");
	m_strServer = _T("127.0.0.1");
	//}}AFX_DATA_INIT
}


void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoginDlg)
	DDX_Text(pDX, IDC_NICKNAME_EDIT, m_strName);
	DDX_Text(pDX, IDC_SERVERIP_EDIT, m_strServer);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	//{{AFX_MSG_MAP(CLoginDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_LOGIN_BUTTON, &CLoginDlg::OnBnClickedLoginButton)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg message handlers

void CLoginDlg::OnBnClickedLoginButton()
{
	// TODO: Add your control notification handler code here
	// TODO: Add extra validation here
	UpdateData();
	if (m_strName == _T("Your name"))
	{
		AfxMessageBox(_T("Input your own name"));
		return;
	}
	//	m_pSocket->m_strName = this->m_strName;
	if(!m_pSocket->Create())
	{
		m_pSocket->Close();
		return;
	}
	if(!m_pSocket->Connect(m_strServer,5151))
	{
		AfxMessageBox(_T("Server is not started:-("));
		m_pSocket->Close();
		return;	
	}

	char nickname[64] = {0};
	WideCharToMultiByte(CP_UTF8, 0, m_strName, m_strName.GetLength(), nickname, sizeof(nickname), 0, 0);

	LoginReq(NULL, nickname);		// step1: rpc->server with parameters

	theApp.m_strName = m_strName;

	CDialog::OnOK();
}

int32 CLoginDlg::LoginReq(void* pClient, const char* nickname)
{
	OutputStream stream;
	uint16 iLength = (uint16)strlen(nickname) + 1;
	stream.Serialize(iLength);
	stream.Serialize(iLength, nickname);			// step2: serialize parameters to a datastream

	SendPacket(pClient, LOGIN, stream.GetDataLength(), stream.GetBuffer());		// step3: retrieve buf and len from datastream

	return 0;
}

int32 CLoginDlg::SendPacket(void* pClient, uint16 iFilterId, uint16 iLen, const char* pBuf)
{
	char outBuf[1024] = {0};
	ServerPacket* pServerPacket = (ServerPacket*)outBuf;
	pServerPacket->m_iLen = iLen;
	pServerPacket->m_iFilterId = iFilterId;
	memcpy(pServerPacket->m_Buf, pBuf, iLen);		// step4: use packet to wrap this buf and add header info

	m_pSocket->Send(outBuf,iLen+SERVER_PACKET_HEAD); // step5: send it

	return 0;
}