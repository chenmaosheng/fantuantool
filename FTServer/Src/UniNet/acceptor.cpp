#include "acceptor.h"

int32 Acceptor::Init(PSOCKADDR_IN pAddr)
{
	int32 rc = 0;
	DWORD val = 0;

	// initialize acceptor's udp socket
	m_Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	_ASSERT(m_Socket != INVALID_SOCKET);
	if (m_Socket == INVALID_SOCKET)
	{
		UN_LOG_ERR(_T("Create acceptor socket failed, err=%d"), WSAGetLastError());
		return -1;
	}

	// set snd buf and recv buf to 0, it's said that it must improve the performance
	setsockopt(m_Socket, SOL_SOCKET, SO_RCVBUF, (const char *)&val, sizeof(val));
	setsockopt(m_Socket, SOL_SOCKET, SO_SNDBUF, (const char *)&val, sizeof(val));

	val = 1;
	ioctlsocket(m_Socket, FIONBIO, &val);
	setsockopt(m_Socket, SOL_SOCKET, SO_BROADCAST, (const char*)&val, sizeof(val));

	UN_LOG_STT(_T("Create and configure acceptor socket"));

	// bind acceptor's socket to assigned ip address
	rc = bind(m_Socket, (sockaddr*)pAddr, sizeof(*pAddr));
	_ASSERT(rc == 0);
	if (rc != 0)
	{
		UN_LOG_ERR(_T("bind to address failed, err=%d"), rc);
		return -5;
	}

	UN_LOG_STT(_T("Initialize acceptor success"));

	return 0;
}

void Acceptor::Destroy()
{
	// close the accept socket
	if (m_Socket != INVALID_SOCKET)
	{
		closesocket(m_Socket);
	}

	UN_LOG_STT(_T("Destroy acceptor success"));
}