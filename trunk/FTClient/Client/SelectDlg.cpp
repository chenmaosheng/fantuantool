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
	DDX_Control(pDX, IDC_AVATAR_LIST, m_AvatarList);
}


BEGIN_MESSAGE_MAP(CSelectDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CSelectDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK2, &CSelectDlg::OnBnClickedOk2)
	ON_BN_CLICKED(IDOK, &CSelectDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSelectDlg message handlers

void CSelectDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	m_pClientLogic->Logout();
}

void CSelectDlg::OnBnClickedOk2()
{
	// TODO: Add your control notification handler code here
	int size = m_AvatarList.GetCount();
	if (size < AVATARCOUNT_MAX)
	{
		m_pClientLogic->AvatarCreate();
	}
	else
	{
		MessageBox(_T("No vacant for new avatar"));
	}
}

void CSelectDlg::ReceiveAvatarList(int32 iRet, uint8 iAvatarCount, const ftdAvatar *pAvatar)
{
	UpdateData(TRUE);
	m_AvatarList.ResetContent();
	if (iRet == 0)
	{
		for (uint8 i = 0; i < iAvatarCount; ++i)
		{
			const ftdAvatar& avatar = pAvatar[i];
			TCHAR strAvatarName[AVATARNAME_MAX+1] = {0};
			Char2WChar(avatar.m_strAvatarName, strAvatarName, AVATARNAME_MAX+1);
			m_AvatarList.InsertString(i, strAvatarName);
		}
	}
	UpdateData(FALSE);
}

void CSelectDlg::ReceiveAvatarCreate(int32 iRet, const ftdAvatar& newAvatar)
{
	UpdateData(TRUE);
	if (iRet == 0)
	{
		TCHAR strAvatarName[AVATARNAME_MAX+1] = {0};
		Char2WChar(newAvatar.m_strAvatarName, strAvatarName, AVATARNAME_MAX+1);
		int size = m_AvatarList.GetCount();
		m_AvatarList.InsertString(size, strAvatarName);
	}
	UpdateData(FALSE);
}

void CSelectDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CString avatarName;
	int index = m_AvatarList.GetCurSel();
	if(index == LB_ERR)
	{
		MessageBox(_T("Select avatar"));
	}
	else
	{
		m_AvatarList.GetText(index, avatarName);
		m_pClientLogic->RequestSelectAvatar(avatarName);
	}

	UpdateData(FALSE);
}
