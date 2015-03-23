#ifndef _H_EASY_BASESESSION
#define _H_EASY_BASESESSION

#include "common.h"

class EasyBaseServer;
struct _EasyPacket;
struct _EasyConnection;
class EasyBaseSession
{
public:
	EasyBaseSession();
	virtual ~EasyBaseSession();

	// handle event from io operation
	virtual int32	OnConnection(ConnID connId);
	virtual void	OnDisconnect();
	virtual void	OnData(uint16 iLen, char* pBuf);
	virtual void	Disconnect();
	virtual int32	SendData(uint16 iTypeId, uint16 iLen, const char* pData);

protected:
	// handle server packet which is analyzed from received buffer
	int32			_HandlePacket(struct _EasyPacket*);
	
public:
	uint32			m_iSessionId;			// sessionid
	struct _EasyConnection* m_pConnection;	// connection from network
	
private:	
	uint16			m_iRecvBufLen;			// received buffer length from client
	char			m_RecvBuf[MAX_BUFFER];	// received buffer from client
	static uint32	m_iSessionIdStart;
};

#endif
