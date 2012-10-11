#pragma once


// CSelectDlg dialog

class CSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectDlg)

public:
	CSelectDlg(ClientLogic *pClientLogic,CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectDlg();

	ClientLogic *m_pClientLogic;

// Dialog Data
	enum { IDD = IDD_SELECT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk2();
};
