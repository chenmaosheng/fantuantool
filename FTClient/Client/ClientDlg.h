// ClientDlg.h : header file
//

#pragma once

#include <vector>
#include "type.h"

// CClientDlg dialog
class ClientLogic;
class CClientDlg : public CDialog
{
// Construction
public:
	CClientDlg(ClientLogic *pClientLogic,CWnd* pParent = NULL);	// standard constructor
	BOOL GetMessage(uint64 iAvatarId, TCHAR* strMessage);
	void UpdateUser(uint64 iAvatarId, TCHAR* strAvatarName);
	void DeleteUser(uint64 iAvatarId);
	
	void ToTray();

// Dialog Data
	enum { IDD = IDD_CHAT_DIALOG };
	NOTIFYICONDATA nid;
	CListBox m_UserList;
	CEdit m_MessageList;
	CString m_strMessage;
	CString m_strName;
	CString m_strServer;
	ClientLogic *m_pClientLogic;
	std::vector< std::pair<uint64, CString> > m_AvatarList;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	BOOL PreTranslateMessage(MSG*   pMsg);
	

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnNotifyIcon(WPARAM wParam,LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();

public:
	afx_msg void OnBnClickedSendButton();
	afx_msg void OnBnClickedCloseButton();
	afx_msg void OnFileExit();
};
