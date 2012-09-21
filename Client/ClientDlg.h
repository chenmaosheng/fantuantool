// ClientDlg.h : header file
//

#pragma once

#include <vector>

// CClientDlg dialog
class CClientSocket;
class CClientDlg : public CDialog
{
// Construction
public:
	CClientDlg(CClientSocket *p_Socket,CWnd* pParent = NULL);	// standard constructor
	BOOL GetMessage(char* message, int length);
	void UpdateUser(char* nickname, int sessionId, int length);
	void DeleteUser(int connId);

	void ToTray();

// Dialog Data
	enum { IDD = IDD_CHAT_DIALOG };
	NOTIFYICONDATA nid;
	CListBox m_UserList;
	CEdit m_MessageList;
	CString m_strMessage;
	CString m_strName;
	CString m_strServer;
	CClientSocket *m_pSocket;
	std::vector< std::pair<int, CString> > m_users;

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
};
