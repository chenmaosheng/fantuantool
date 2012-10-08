// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "LoginDlg.h"
#include "targetver.h"

#include "client_base.h"
#include "client_config.h"
#include "event.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CClientApp theApp;
/////////////////////////////////////////////////////////////////////////////



// CLoginDlg dialog


CLoginDlg::CLoginDlg(ClientBase *pClientBase, CWnd* pParent /*=NULL*/)
: CDialog(CLoginDlg::IDD, pParent)

{
	ASSERT(pClientBase);
	m_pClientBase = pClientBase;
	//{{AFX_DATA_INIT(CLoginDlg)
	m_strName = _T("Your name");
	m_strPassword = _T("");
	//}}AFX_DATA_INIT

	// register event handler
	m_pClientBase->SetDisconnectEvent(new ClientBase::DisconnectEvent(&CLoginDlg::OnDisconnect, this));
}


void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoginDlg)
	DDX_Text(pDX, IDC_NICKNAME_EDIT, m_strName);
	DDX_Text(pDX, IDC_PASSWORD_EDIT, m_strPassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	//{{AFX_MSG_MAP(CLoginDlg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_LOGIN_BUTTON, &CLoginDlg::OnBnClickedLoginButton)
	ON_BN_CLICKED(ID_CREATE_BUTTON, &CLoginDlg::OnBnClickedCreateButton)
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

	TCHAR strToken[MAX_TOKEN_LEN] = {0};
	swprintf_s(strToken, MAX_TOKEN_LEN, _T("%s;%s"), m_strName, m_strPassword);
	char strUTF8[MAX_TOKEN_LEN] = {0};
	WChar2Char(strToken, MAX_TOKEN_LEN, strUTF8, MAX_TOKEN_LEN);

	m_pClientBase->Login(inet_addr(g_pClientConfig->GetLoginHost()), g_pClientConfig->GetLoginPort(), strUTF8);

	CDialog::OnOK();
}

void CLoginDlg::OnBnClickedCreateButton()
{
	ShellExecute(NULL, _T("open"), g_pClientConfig->GetCreateAccountPage(), NULL, NULL, SW_SHOWNORMAL);
}

void CLoginDlg::OnDisconnect()
{

}
