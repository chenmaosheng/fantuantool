// CreateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Client.h"
#include "CreateDlg.h"
#include "ClientLogic.h"

#include "client_base.h"


// CCreateDlg dialog

IMPLEMENT_DYNAMIC(CCreateDlg, CDialog)

CCreateDlg::CCreateDlg(ClientLogic *pClientLogic,CWnd* pParent /*=NULL*/)
	: CDialog(CCreateDlg::IDD, pParent)
{

}

CCreateDlg::~CCreateDlg()
{
}

void CCreateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCreateDlg, CDialog)
END_MESSAGE_MAP()


// CCreateDlg message handlers
