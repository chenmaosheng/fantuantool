// SelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "SelectDlg.h"
#include "ClientLogic.h"

#include "client_base.h"


// CSelectDlg dialog

IMPLEMENT_DYNAMIC(CSelectDlg, CDialog)

CSelectDlg::CSelectDlg(ClientLogic *pClientLogic,CWnd* pParent /*=NULL*/)
	: CDialog(CSelectDlg::IDD, pParent)
{
	m_pClientLogic = pClientLogic;
}

CSelectDlg::~CSelectDlg()
{
}

void CSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSelectDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CSelectDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CSelectDlg message handlers

void CSelectDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	m_pClientLogic->Logout();
}
