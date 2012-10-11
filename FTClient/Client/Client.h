// Client.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include <vector>

enum
{
	LOGIN_PAGE,
	SELECT_PAGE,
	CREATE_PAGE,
	CLIENT_PAGE,
};

enum
{
	LOGIN_SUCCESS_EVENT,
	CREATE_REQUEST_EVENT,
	CREATE_SUCCESS_EVENT,
	SELECT_REQUEST_EVENT,
	BACK_EVENT,
	LOGOUT_SUCCESS_EVENT,
};

struct PageState
{
	int m_iCurrPage;
	int m_iEvent;
	int m_iNewPage;

	PageState(int iCurrPage, int iEvent, int iNewPage) : m_iCurrPage(iCurrPage), m_iEvent(iEvent), m_iNewPage(iNewPage){}
};


// CClientApp:
// See Client.cpp for the implementation of this class
//
class CClientDlg;
class CLoginDlg;
class CSelectDlg;
class CCreateDlg;
class ClientLogic;
class CClientApp : public CWinApp
{
public:
	CClientApp();

// Overrides
public:
	virtual BOOL InitInstance();

	void TriggerPageEvent(int);
	
// Implementation

	DECLARE_MESSAGE_MAP()

private:
	void ShowDialog(int);
	void DeleteDialog(int);

public:
	std::vector<PageState> m_vPageState;
	int m_iCurrPage;
	CLoginDlg* m_pLoginDlg;
	CClientDlg* m_pClientDlg;
	CSelectDlg* m_pSelectDlg;
	CCreateDlg* m_pCreateDlg;
	ClientLogic* m_pClientLogic;
};

extern CClientApp theApp;