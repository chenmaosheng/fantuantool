// ChannelDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "ChannelDlg.h"
#include "ClientLogic.h"

#include "client_base.h"

// CChannelDialog dialog

IMPLEMENT_DYNAMIC(CChannelDlg, CDialog)

CChannelDlg::CChannelDlg(ClientLogic *pClientLogic,CWnd* pParent /*=NULL*/)
	: CDialog(CChannelDlg::IDD, pParent)
{
	m_pClientLogic = pClientLogic;
}

CChannelDlg::~CChannelDlg()
{
}

void CChannelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHANNEL_LIST, m_ChannelList);
}


BEGIN_MESSAGE_MAP(CChannelDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CChannelDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CChannelDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

void CChannelDlg::ReceiveChannelList(uint8 iChannelCount, const ftdChannelData* arrayData)
{
	UpdateData(TRUE);
	m_ChannelList.ResetContent();
	for (uint8 i = 0; i < iChannelCount; ++i)
	{
		const ftdChannelData& data = arrayData[i];
		TCHAR strChannelName[CHANNELNAME_MAX+1] = {0};
		Char2WChar(data.m_strChannelName, strChannelName, CHANNELNAME_MAX+1);
		m_ChannelList.InsertString(i, strChannelName);
	}
	UpdateData(FALSE);
}


// CChannelDialog message handlers

void CChannelDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	CString channelName;
	int index = m_ChannelList.GetCurSel();
	if(index == LB_ERR)
	{
		MessageBox(_T("Select channel"));
	}
	else
	{
		m_ChannelList.GetText(index, channelName);
		m_pClientLogic->RequestSelectChannel(channelName);
	}

	UpdateData(FALSE);
}

void CChannelDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	m_pClientLogic->Logout();
}
