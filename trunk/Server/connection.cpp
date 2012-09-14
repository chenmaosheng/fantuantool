#include "connection.h"

Connection::Connection()
{
	connected_ = 0;
	sendContext_.operation_type_ = OPERATION_SEND;
	recvContext_.operation_type_ = OPERATION_RECV;
}

void Connection::AsyncRecv()
{
	recvContext_.Reset();
	recvContext_.connection_ = this;
	recvContext_.wsabuf_.len = 1024;
	DWORD dwXfer, dwFlag = 0;
	if (WSARecv(socket_, &recvContext_.wsabuf_, 1, &dwXfer, &dwFlag, &recvContext_.overlapped_, NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			return;
		}
	}
}

void Connection::AsyncSend(int len, char* buf)
{
	sendContext_.Reset();
	sendContext_.connection_ = this;
	memcpy(sendContext_.buffer_, buf, len);
	sendContext_.buffer_[strlen(sendContext_.buffer_)] = '\0';
	sendContext_.wsabuf_.len = len;
	sendContext_.wsabuf_.buf = sendContext_.buffer_;
	DWORD dwXfer;
	if (WSASend(socket_, &sendContext_.wsabuf_, 1, &dwXfer, 0, &sendContext_.overlapped_, NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			return;
		}
	}
}