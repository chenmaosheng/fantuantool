#if !defined(AFX_CLIENTSOCKET_H__DDE7D64D_BB03_433F_9289_9E65E096B369__INCLUDED_)
#define AFX_CLIENTSOCKET_H__DDE7D64D_BB03_433F_9289_9E65E096B369__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ClientSocket.h : header file
//
#include "packet.h"

class CClientDlg;
enum
{
	NOT_CONNECT,
	CONNECTED,
	LOGGEDIN,
};
/////////////////////////////////////////////////////////////////////////////
// CClientSocket command target
class CClientSocket : public CSocket
{
	// Attributes
public:

	// Operations
public:
	CClientSocket();
	virtual ~CClientSocket();

	// Overrides
public:
	CClientDlg *chatDlg;
	CString m_strName;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientSocket)
public:
	virtual void OnReceive(int nErrorCode);
	int32 HandlePacket(ServerPacket*);
	void SetState(int32 iState){m_iState = iState;}
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CClientSocket)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	// Implementation
protected:
	uint32 m_iRecvBufLen;
	char m_RecvBuf[MAX_OUTPUT_BUFFER];
	ServerPacket m_Packet;
	int32 m_iState;

private:
	int VersionReq(void* pClient, int iVersion);
	int SendPacket(void* pClient, UINT16 iTypeId, UINT16 iLen, const char* pBuf);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTSOCKET_H__DDE7D64D_BB03_433F_9289_9E65E096B369__INCLUDED_)
