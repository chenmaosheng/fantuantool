// CreateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "CreateDlg.h"
#include "ClientLogic.h"

#include "client_base.h"


// CCreateDlg dialog

IMPLEMENT_DYNAMIC(CCreateDlg, CDialog)

CCreateDlg::CCreateDlg(ClientLogic *pClientLogic,CWnd* pParent /*=NULL*/)
	: CDialog(CCreateDlg::IDD, pParent)
{
	m_pClientLogic = pClientLogic;
}

CCreateDlg::~CCreateDlg()
{
}

void CCreateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NICK_EDIT, m_strNickname);
}


BEGIN_MESSAGE_MAP(CCreateDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CCreateDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CCreateDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCreateDlg message handlers

void CCreateDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	m_pClientLogic->BackToSelect();
}

void CCreateDlg::OnBnClickedOk()
{
	UpdateData();
	// TODO: Add your control notification handler code here
	if (m_strNickname != _T(""))
	{
		m_pClientLogic->RequestCreateAvatar(m_strNickname);
	}
	
}
