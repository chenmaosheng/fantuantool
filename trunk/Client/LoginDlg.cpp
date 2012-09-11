// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "LoginDlg.h"
#include "ClientSocket.h"
#include "targetver.h"
#include "Command.h"
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
	m_strName = _T("tsubasa");
	m_strServer = _T("192.168.1.4");
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

	LoginPkt pkt;
	WideCharToMultiByte(CP_UTF8, 0, m_strName, m_strName.GetLength(), pkt.nickname, sizeof(pkt.nickname), 0, 0);

	pkt.len = (int)strlen(pkt.nickname)+1+sizeof(pkt.index);
	m_pSocket->Send((char *)&pkt,sizeof(LoginPkt));

	theApp.m_strName = m_strName;

	CDialog::OnOK();
}
