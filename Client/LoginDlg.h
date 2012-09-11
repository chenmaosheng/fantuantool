#if !defined(AFX_LOGINDLG_H__33DB2C16_B9F7_4CDA_9291_CBDA65D6E03B__INCLUDED_)
#define AFX_LOGINDLG_H__33DB2C16_B9F7_4CDA_9291_CBDA65D6E03B__INCLUDED_

#include "ClientSocket.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoginDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog

class CLoginDlg : public CDialog
{
	// Construction
public:
	CLoginDlg(CClientSocket *p_Socket,CWnd* pParent=NULL);   // standard constructor
	CClientSocket *m_pSocket;
	// Dialog Data
	//{{AFX_DATA(CLoginDlg)
	enum { IDD = IDD_LOGIN_DIALOG };
	CString	m_strName;
	CString	m_strServer;
	//}}AFX_DATA


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoginDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoginDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLoginButton();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGINDLG_H__33DB2C16_B9F7_4CDA_9291_CBDA65D6E03B__INCLUDED_)
