#include "worker.h"
#include "starnet.h"
#include "context_pool.h"
#include "acceptor.h"
#include "peer_packet.h"
#include "connection.h"

bool CALLBACK OnConnection(ConnID connId)
{
	return true;
}

void CALLBACK OnDisconnect(ConnID connId)
{
}

void CALLBACK OnData(ConnID connId, uint32& iAvailLen, char* pBuf, uint32& iOffset)
{
	char* recvBuf = pBuf + iOffset;
}

void CALLBACK OnConnectFailed(void*)
{
}


int main()
{
	StarNet::Init();

	// set the event handler
	static Handler handler;
	handler.OnConnection = &OnConnection;
	handler.OnDisconnect = &OnDisconnect;
	handler.OnData = &OnData;
	handler.OnConnectFailed = &OnConnectFailed;

	// create iocp worker
	Worker* m_pWorker = Worker::CreateWorker(1);
	if (!m_pWorker)
	{
		return -1;
	}

	// create pool of context
	ContextPool* m_pContextPool = ContextPool::CreateContextPool(MAX_INPUT_BUFFER, MAX_OUTPUT_BUFFER);
	if (!m_pContextPool)
	{
		return -2;
	}

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(0);
	addr.sin_port = htons(8082);
	// create acceptor to receive connection
	Acceptor* m_pAcceptor = Acceptor::CreateAcceptor(&addr, m_pWorker, m_pContextPool, &handler);
	if (!m_pAcceptor)
	{
		return -3;
	}

	m_pAcceptor->Start();

	while (true)
	{
		Sleep(1000);
	}
	
	return 0;
}