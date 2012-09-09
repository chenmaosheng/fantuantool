// Client.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CClientApp:
// See Client.cpp for the implementation of this class
//
class CClientDlg;
class CClientApp : public CWinApp
{
public:
	CClientApp();

// Overrides
public:
	virtual BOOL InitInstance();
	void SendToServer(const TCHAR* message, size_t length);

// Implementation

	DECLARE_MESSAGE_MAP()

public:
	CClientDlg*		 dlg;
private:
	WSADATA          wsd;
    SOCKET           s;
    int              rc;                            // return code
	SOCKADDR_IN		 addr;
	HANDLE			 recvHandler;
};

extern CClientApp theApp;