// Client.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <process.h>

#pragma   comment(lib,   "ws2_32.lib") 

unsigned int WINAPI RecvThread(void* s);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CClientApp

BEGIN_MESSAGE_MAP(CClientApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CClientApp construction

CClientApp::CClientApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	// Load Winsock
    WSAStartup(MAKEWORD(2,2), &wsd);

    // create the socket
    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	addr.sin_addr.s_addr = inet_addr("192.168.1.4");
	addr.sin_family = AF_INET;
	addr.sin_port = htons(5150);
    rc = connect(s, (SOCKADDR*)&addr, sizeof(addr));

	recvHandler = (HANDLE)_beginthreadex(NULL, 0, RecvThread, &s, 0, NULL);
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

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	dlg = new CClientDlg();
	m_pMainWnd = dlg;
	INT_PTR nResponse = dlg->DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CClientApp::SendToServer(const TCHAR* message, size_t length)
{
	char buf[128] = {0};
	WideCharToMultiByte(CP_UTF8, 0, message, length, buf, 128, 0, 0);
	send(s, buf, strlen(buf)+1, 0);
}

unsigned int WINAPI RecvThread(void* s)
{
	SOCKET* recvSocket = (SOCKET*)s;
	int rc = 0;
	char recvBuf[128] = {0};
	while (1)
	{
		rc = recv(*recvSocket, recvBuf, 128, 0);
		if (rc == 0)
		{
			//printf("server closed\n");
			theApp.dlg->Write("Server closed", 20);
			break;
		}
		else
		if (rc == SOCKET_ERROR)
		{
			//printf("socket error\n");
			theApp.dlg->Write("Socket error", 20);
			break;
		}
		else
		{
			//printf("output: %s\n", recvBuf);
			theApp.dlg->Write(recvBuf, strlen(recvBuf)+1);
		}
	}

	return 0;
}