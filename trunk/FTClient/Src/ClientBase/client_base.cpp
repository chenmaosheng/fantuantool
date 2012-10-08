#include "client_base.h"
#include "worker.h"
#include "minidump.h"
#include "simplenet.h"
#include "connector.h"
#include "handler.h"
#include "memory_pool.h"
#include "version.h"

#include "login_client_send.h"

ClientBase::ClientBase()
{
	m_pWorker = NULL;
	m_pLogSystem = NULL;
	m_pConnector = NULL;

	m_iState = NOT_CONNECT;
}

ClientBase::~ClientBase()
{
}

int32 ClientBase::Init()
{
	int32 iRet = 0;

	// initialize minidump
	Minidump::Init(_T("log"));

	// set the min and max of memory pool object
	MEMORY_POOL_INIT(MEMORY_OBJECT_MIN, MEMORY_OBJECT_MAX);

	iRet = SimpleNet::Init();
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("Initialize SimpleNet failed"));
		return -3;
	}

	m_pWorker = Worker::CreateWorker();

	return 0;
}

void ClientBase::Destroy()
{
	SimpleNet::Destroy();
}

void ClientBase::Login(const TCHAR *strHost, uint16 iPort, const TCHAR *strToken)
{
	if (m_pConnector)
	{
		Connector::Close(m_pConnector);
		m_pConnector = NULL;
	}

	WChar2Char(strToken, m_TokenPacket.m_TokenBuf, MAX_TOKEN_LEN);
	m_TokenPacket.m_iTokenLen = strlen(m_TokenPacket.m_TokenBuf) + 1;

	m_SockAddr.sin_family = AF_INET;
	m_SockAddr.sin_port = htons(iPort);
	char strUTF8[MAX_PATH] = {0};
	WChar2Char(strHost, strUTF8, MAX_PATH);
	m_SockAddr.sin_addr.s_addr = inet_addr(strUTF8);

	static Handler handler;
	handler.OnConnection = &OnConnection;
	handler.OnDisconnect = &OnDisconnect;
	handler.OnData = &OnData;
	handler.OnConnectFailed = &OnConnectFailed;

	Connector::Connect(&m_SockAddr, &handler, m_pWorker, this);
}

void ClientBase::OnClientData(uint32 iLen, char* pBuf)
{
	uint32 iCopyLen = 0;
	int32 iRet = 0;
	// check if peer client is connected
	if (!m_pConnector->IsConnected())
	{
		return;
	}

	do 
	{
		// check if received buffer is not enough
		// if that, we should split the incoming buffer and handle the rest of them
		if (m_iRecvBufLen + iLen <= sizeof(m_RecvBuf))
		{
			memcpy(m_RecvBuf + m_iRecvBufLen, pBuf, iLen);
			m_iRecvBufLen += iLen;
			pBuf += iLen;
			iLen = 0;
		}
		else
		{
			iCopyLen = m_iRecvBufLen + iLen - sizeof(m_RecvBuf);
			memcpy(m_RecvBuf + m_iRecvBufLen, pBuf, iCopyLen);
			pBuf += iCopyLen;
			iLen -= iCopyLen;
			m_iRecvBufLen += iCopyLen;
		}

		if (m_iState == CONNECTED)
		{
			while (m_iRecvBufLen >= strlen(g_LoggedInNtf))
			{
				iRet = HandleLoginPacket(m_iRecvBufLen, m_RecvBuf);
				if (iRet < 0)
				{
					return;
				}

				if (m_iRecvBufLen > strlen(g_LoggedInNtf))
				{
					memmove(m_RecvBuf, m_RecvBuf + strlen(g_LoggedInNtf), m_iRecvBufLen - strlen(g_LoggedInNtf));
				}
				m_iRecvBufLen -= strlen(g_LoggedInNtf);

				if (iRet == 1 && m_iRecvBufLen != 0)
				{
					/*LOG_ERR(LOG_SERVER, _T("sid=%08x why there is other data received"), m_iSessionId);
					return;*/
					m_iRecvBufLen = 0;
				}
			}
		}
		else
		{

		}
	} while (iLen);
}

int32 ClientBase::HandleLoginPacket(uint16 iLen, char *pBuf)
{
	m_iState = LOGGEDIN;

	LoginClientSend::VersionReq(NULL, CLIENT_VERSION);

	return 0;
}

bool CALLBACK ClientBase::OnConnection(ConnID connId)
{
	Connector* pConnector = (Connector*)connId;
	ClientBase* pClientBase = (ClientBase*)pConnector->GetClient();
	pClientBase->m_pConnector = pConnector;
	pClientBase->m_iState = CONNECTED;
	pClientBase->m_iRecvBufLen = 0;

	pConnector->Send(pClientBase->m_TokenPacket.m_iTokenLen + sizeof(uint16), (char*)&pClientBase->m_TokenPacket);

	return true;
}

void CALLBACK ClientBase::OnDisconnect(ConnID connId)
{
	Connector* pConnector = (Connector*)connId;
	ClientBase* pClientBase = (ClientBase*)pConnector->GetClient();
	pClientBase->m_iState = DISCONNECTED;
}

void CALLBACK ClientBase::OnData(ConnID connId, uint32 iLen, char* pBuf)
{
	Connector* pConnector = (Connector*)connId;
	ClientBase* pClientBase = (ClientBase*)pConnector->GetClient();
	pClientBase->OnClientData(iLen, pBuf);
}

void CALLBACK ClientBase::OnConnectFailed(void* pClient)
{
	ClientBase* pClientBase = (ClientBase*)pClient;
	if (pClientBase->m_iState == DESTORYING)
	{
		pClientBase->m_iState = NOT_CONNECT;
	}
	else
	if (pClientBase->m_iState == CONNECTING)
	{
		pClientBase->m_iState = CONNECT_FAILED;
	}
}
