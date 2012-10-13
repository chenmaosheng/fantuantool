#pragma once

#include "ClientLogic.h"
// CChannelDialog dialog

class CChannelDlg : public CDialog
{
	DECLARE_DYNAMIC(CChannelDlg)

public:
	CChannelDlg(ClientLogic *pClientLogic,CWnd* pParent = NULL);   // standard constructor
	virtual ~CChannelDlg();

	ClientLogic *m_pClientLogic;

	void ReceiveChannelList(uint8 iChannelCount, const ftdChannelData* arrayData);

// Dialog Data
	enum { IDD = IDD_CHANNEL_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

	CListBox m_ChannelList;
	afx_msg void OnBnClickedCancel();
};
