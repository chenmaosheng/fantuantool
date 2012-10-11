#pragma once


// CCreateDlg dialog

class CCreateDlg : public CDialog
{
	DECLARE_DYNAMIC(CCreateDlg)

public:
	CCreateDlg(ClientLogic *pClientLogic,CWnd* pParent = NULL);   // standard constructor
	virtual ~CCreateDlg();

// Dialog Data
	enum { IDD = IDD_CREATE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
