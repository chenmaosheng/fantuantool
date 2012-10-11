#pragma once


// CCreateDlg dialog

class CCreateDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateDlg)

public:
	CCreateDlg(ClientLogic *pClientLogic,CWnd* pParent = NULL);   // standard constructor
	virtual ~CCreateDlg();

	ClientLogic *m_pClientLogic;
	CString m_strNickname;

// Dialog Data
	enum { IDD = IDD_CREATE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
