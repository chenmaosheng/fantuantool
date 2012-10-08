#include "connector.h"
#include "worker.h"

bool Connector::Connect(PSOCKADDR_IN addr, void* client)
{
	int32 rc = 0;
	client_ = client;

	rc = connect(socket_, (sockaddr*)addr, sizeof(*addr));
	if (rc == SOCKET_ERROR)
	{
		SN_LOG_ERR(_T("Connect failed"));
		handler_.OnConnectFailed(client);
		return false;
	}

	connected_ = 1;
	handler_.OnConnection((ConnID)this);

	SN_LOG_DBG(_T("Connect success"));

	return true;
}

void Connector::Disconnect()
{
	closesocket(socket_);
}

void Connector::Recv()
{
	char buffer[MAX_OUTPUT_BUFFER] = {0};
	int32 iLen = 0;
	iLen = recv(socket_, buffer, MAX_OUTPUT_BUFFER, 0);
	if (iLen == SOCKET_ERROR)
	{
		SN_LOG_ERR(_T("Recv failed"));
	}
	else
	if (iLen == 0)
	{
		handler_.OnDisconnect((ConnID)this);
	}
	else
	{
		handler_.OnData((ConnID)this, (uint32)iLen, buffer);
	}
}

void Connector::Send(int iLen, char* pBuf)
{
	int32 rc = 0;
	rc = send(socket_, pBuf, iLen, 0);
	if (rc == SOCKET_ERROR)
	{
		SN_LOG_ERR(_T("Send failed"));
	}
}

void Connector::SetClient(void* pClient)
{
	client_ = pClient;
}

void* Connector::GetClient()
{
	return client_;
}

bool Connector::IsConnected()
{
	return connected_ ? true : false;
}

Connector* Connector::Create(Handler* pHandler, Worker* pWorker)
{
	Connector* pConnector = (Connector*)_aligned_malloc(sizeof(Connector), MEMORY_ALLOCATION_ALIGNMENT);
	if (pConnector)
	{
		// initialize connection's tcp socket
		pConnector->socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (pConnector->socket_ != INVALID_SOCKET)
		{
			u_long non_blocking = 1;
			//ioctlsocket(pConnector->socket_, FIONBIO, &non_blocking);
			DWORD val = 0;

			// set snd buf and recv buf to 0, it's said that it must improve the performance
			setsockopt(pConnector->socket_, SOL_SOCKET, SO_RCVBUF, (const char *)&val, sizeof(val));
			setsockopt(pConnector->socket_, SOL_SOCKET, SO_SNDBUF, (const char *)&val, sizeof(val));

			val = 1;
			setsockopt(pConnector->socket_, SOL_SOCKET, SO_REUSEADDR, (const char *)&val, sizeof(val));
			setsockopt(pConnector->socket_, IPPROTO_TCP, TCP_NODELAY, (const char *)&val, sizeof(val));

			pConnector->handler_ = *pHandler;
			pConnector->worker_ = pWorker;
			pConnector->connected_ = 0;
			pWorker->m_pConnector = pConnector;

			SOCKADDR_IN addr;
			ZeroMemory(&addr, sizeof(addr));
			addr.sin_family = AF_INET;
			if (bind(pConnector->socket_, (sockaddr*)&addr, sizeof(addr)) == 0)
			{
				SN_LOG_DBG(_T("Create and configure connection in client side"));
				return pConnector;
			}
		}
		else
		{
			_aligned_free(pConnector);
		}
	}

	SN_LOG_ERR(_T("Create connector failed, err=%d"), GetLastError());

	return NULL;
}

bool Connector::Connect(PSOCKADDR_IN pAddr, Handler* pHandler, Worker* pWorker, void* pClient)
{
	Connector* pConnector = Create(pHandler, pWorker);
	if (pConnector)
	{
		if (pConnector->Connect(pAddr, pClient))
		{
			return true;
		}

		Delete(pConnector);
	}

	return false;
}

void Connector::Close(Connector* pConnector)
{
	Delete(pConnector);
	
	SN_LOG_DBG(_T("Close connection success"));
}

void Connector::Delete(Connector* pConnector)
{
	closesocket(pConnector->socket_);
	_aligned_free(pConnector);
	pConnector = NULL;

	SN_LOG_DBG(_T("Close socket success"));
}