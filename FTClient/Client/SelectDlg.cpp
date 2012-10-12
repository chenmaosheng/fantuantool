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
	m_pClientLogic->AvatarCreate();
}

void CSelectDlg::ReceiveAvatarList(int32 iRet, uint8 iAvatarCount, const ftdAvatar *pAvatar)
{
	m_AvatarList.DeleteAllItems();
	if (iRet)
	{
		for (uint8 i = 0; i < iAvatarCount; ++i)
		{
			const ftdAvatar& avatar = pAvatar[i];
			TCHAR strAvatarName[AVATARNAME_MAX+1] = {0};
			Char2WChar(avatar.m_strAvatarName, AVATARNAME_MAX+1, strAvatarName, AVATARNAME_MAX+1);
			m_AvatarList.InsertColumn(i, strAvatarName);
		}
	}
}
void CSelectDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	POSITION pos = m_AvatarList.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		MessageBox(_T("Select avatar"));
	}
	else
	{
		int index = m_AvatarList.GetNextSelectedItem(pos);
		CString avatarName = m_AvatarList.GetItemText(index, 0);
		m_pClientLogic->RequestSelectAvatar(avatarName);
	}
}
