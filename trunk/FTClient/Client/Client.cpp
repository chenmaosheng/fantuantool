// Client.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "LoginDlg.h"
#include "SelectDlg.h"
#include "CreateDlg.h"
#include "ClientLogic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientApp

BEGIN_MESSAGE_MAP(CClientApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CClientApp construction

CClientApp::CClientApp() : m_pCreateDlg(NULL), m_pLoginDlg(NULL), m_pClientDlg(NULL), m_pSelectDlg(NULL), m_pClientLogic(NULL)
{
	m_vPageState.push_back(PageState(LOGIN_PAGE, LOGIN_SUCCESS_EVENT, SELECT_PAGE));
	
	m_vPageState.push_back(PageState(SELECT_PAGE, CREATE_REQUEST_EVENT, CREATE_PAGE));
	m_vPageState.push_back(PageState(SELECT_PAGE, LOGOUT_SUCCESS_EVENT, LOGIN_PAGE));
	m_vPageState.push_back(PageState(SELECT_PAGE, SELECT_REQUEST_EVENT, CLIENT_PAGE));

	m_vPageState.push_back(PageState(CREATE_PAGE, CREATE_SUCCESS_EVENT, SELECT_PAGE));
	m_vPageState.push_back(PageState(CREATE_PAGE, BACK_EVENT, SELECT_PAGE));

	m_vPageState.push_back(PageState(CLIENT_PAGE, LOGOUT_SUCCESS_EVENT, LOGIN_PAGE));
	m_vPageState.push_back(PageState(CLIENT_PAGE, BACK_EVENT, SELECT_PAGE));
}


// The one and only CClientApp object

CClientApp theApp;


// CClientApp initialization

BOOL CClientApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	g_pClientBase = new ClientBase;
	g_pClientBase->Init();
	m_pClientLogic = new ClientLogic(g_pClientBase);

	m_iCurrPage = LOGIN_PAGE;
	ShowDialog(m_iCurrPage);
	//m_pLoginDlg = new CLoginDlg(m_pClientBase);
	//if (m_pLoginDlg->DoModal() == IDCANCEL)
	//{
	//	delete m_pClientBase;
	//	delete m_pLoginDlg;
	//	return false;
	//}
	//else
	//{
	//	delete m_pLoginDlg;
	//}

	//CClientDlg* dlg = new CClientDlg(m_pClientBase);
	//m_pMainWnd = dlg;
	//INT_PTR nResponse = dlg->DoModal();
	//if (nResponse == IDOK)
	//{
	//	// TODO: Place code here to handle when the dialog is
	//	//  dismissed with OK
	//}
	//else if (nResponse == IDCANCEL)
	//{
	//	// TODO: Place code here to handle when the dialog is
	//	//  dismissed with Cancel
	//}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CClientApp::TriggerPageEvent(int iEvent)
{
	int iLastCurrPage = m_iCurrPage;
	std::vector<PageState>::iterator it = m_vPageState.begin();
	while (it != m_vPageState.end())
	{
		if ((*it).m_iCurrPage == m_iCurrPage &&
			(*it).m_iEvent == iEvent)
		{
			m_iCurrPage = (*it).m_iNewPage;
			break;
		}

		++it;
	}

	if (iLastCurrPage != m_iCurrPage)
	{
		DeleteDialog(iLastCurrPage);
		ShowDialog(m_iCurrPage);
	}
}

void CClientApp::ShowDialog(int iPage)
{
	switch(iPage)
	{
	case LOGIN_PAGE:
		if (!m_pLoginDlg)
		{
			m_pLoginDlg = new CLoginDlg(m_pClientLogic);
			m_pLoginDlg->DoModal();
		}
		m_pLoginDlg->ShowWindow(SW_NORMAL);
		
		break;

	case SELECT_PAGE:
		if (!m_pSelectDlg)
		{
			m_pSelectDlg = new CSelectDlg(m_pClientLogic);
			m_pSelectDlg->DoModal();
		}
		m_pSelectDlg->ShowWindow(SW_NORMAL);
		
		break;

	case CREATE_PAGE:
		if (!m_pCreateDlg)
		{
			m_pCreateDlg = new CCreateDlg(m_pClientLogic);
			m_pCreateDlg->DoModal();
		}
		m_pCreateDlg->ShowWindow(SW_NORMAL);
		
		break;

	case CLIENT_PAGE:
		if (!m_pCreateDlg)
		{
			m_pClientDlg = new CClientDlg(m_pClientLogic);
			m_pClientDlg->DoModal();
		}
		m_pClientDlg->ShowWindow(SW_NORMAL);
		m_pMainWnd = m_pClientDlg;
		
		break;
	}
}

void CClientApp::DeleteDialog(int iPage)
{
	switch(iPage)
	{
	case LOGIN_PAGE:
		m_pLoginDlg->ShowWindow(SW_HIDE);
		break;

	case SELECT_PAGE:
		m_pSelectDlg->ShowWindow(SW_HIDE);
		break;

	case CREATE_PAGE:
		m_pCreateDlg->ShowWindow(SW_HIDE);
		break;

	case CLIENT_PAGE:
		m_pClientDlg->ShowWindow(SW_HIDE);
		break;
	}
}