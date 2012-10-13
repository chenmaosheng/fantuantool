// LoginDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "LoginDlg.h"
#include "targetver.h"
#include "ClientLogic.h"

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


CLoginDlg::CLoginDlg(ClientLogic *pClientLogic, CWnd* pParent /*=NULL*/)
: CDialog(CLoginDlg::IDD, pParent)

{
	m_pClientLogic = pClientLogic;
	//{{AFX_DATA_INIT(CLoginDlg)
	m_strName = g_pClientConfig->GetAcccountName();
	m_strPassword = g_pClientConfig->GetPassword();
	//}}AFX_DATA_INIT
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
	ON_MESSAGE(WM_TIMER, &CLoginDlg::OnKickIdle)
	ON_BN_CLICKED(IDCANCEL, &CLoginDlg::OnBnClickedCancel)
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

	SetTimer(1, 100, NULL);

	TCHAR strToken[MAX_TOKEN_LEN] = {0};
	swprintf_s(strToken, MAX_TOKEN_LEN, _T("%s;%s"), m_strName, m_strPassword);

	m_pClientLogic->Login(strToken);

	g_pClientConfig->SaveConfig(m_strName, m_strPassword);
}

void CLoginDlg::OnBnClickedCreateButton()
{
	ShellExecute(NULL, _T("open"), g_pClientConfig->GetCreateAccountPage(), NULL, NULL, SW_SHOWNORMAL);
}

void CLoginDlg::OnDisconnect()
{

}

LRESULT CLoginDlg::OnKickIdle(WPARAM wParam, LPARAM lParam)
{
	static bool bAllowed = true;
	if (bAllowed)
	{
		bAllowed = false;
		m_pClientLogic->OnIncomingEvent();
		bAllowed = true;
	}
	return 0;
}
void CLoginDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
