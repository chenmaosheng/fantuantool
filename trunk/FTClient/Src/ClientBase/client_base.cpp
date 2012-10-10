#include "client_base.h"
#include "worker.h"
#include "context_pool.h"
#include "minidump.h"
#include "starnet.h"
#include "connection.h"
#include "handler.h"
#include "memory_pool.h"
#include "version.h"
#include "client_config.h"

#include "login_client_send.h"
#include "gate_client_send.h"

ClientConfig* g_pClientConfig = NULL;

ClientBase::ClientBase()
{
	m_pWorker = NULL;
	m_pLogSystem = NULL;
	m_ConnId = NULL;
	m_bInLogin = true;
	m_iRecvBufLen = 0;
	memset(m_RecvBuf, 0, sizeof(m_RecvBuf));

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

	StarNet::Init();

	// create worker with 1 thread and context pool
	m_pWorker = Worker::CreateWorker(1);
	m_pContextPool = ContextPool::CreateContextPool(MAX_INPUT_BUFFER, MAX_OUTPUT_BUFFER);

	g_pClientConfig = new ClientConfig;
	g_pClientConfig->LoadConfig();

	return 0;
}

void ClientBase::Destroy()
{
	SAFE_DELETE(g_pClientConfig);
	StarNet::Destroy();
}

void ClientBase::Login(uint32 iIP, uint16 iPort, const char *strToken)
{
	if (m_ConnId)
	{
		Connection::Close((Connection*)m_ConnId);
	}

	strcpy_s(m_TokenPacket.m_TokenBuf, MAX_TOKEN_LEN, strToken);
	m_TokenPacket.m_iTokenLen = strlen(m_TokenPacket.m_TokenBuf) + 1;

	m_SockAddr.sin_family = AF_INET;
	m_SockAddr.sin_port = htons(iPort);
	m_SockAddr.sin_addr.s_addr = iIP;

	static Handler handler;
	handler.OnConnection = &OnConnection;
	handler.OnDisconnect = &OnDisconnect;
	handler.OnData = &OnData;
	handler.OnConnectFailed = &OnConnectFailed;

	Connection::Connect(&m_SockAddr, &handler, m_pContextPool, m_pWorker, this);
}

void ClientBase::OnClientData(uint32 iLen, char* pBuf)
{
	uint32 iCopyLen = 0;
	int32 iRet = 0;
	// check if peer client is connected
	Connection* pConnection = (Connection*)m_ConnId;
	if (!pConnection->IsConnected())
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
			while (m_iRecvBufLen >= SERVER_PACKET_HEAD)	// step2: check if buffer is larger than header
			{
				ServerPacket* pServerPacket = (ServerPacket*)m_RecvBuf;
				uint16 iFullLength = pServerPacket->m_iLen+SERVER_PACKET_HEAD;
				if (m_iRecvBufLen >= iFullLength)	// step3: cut specific size from received buffer
				{
					iRet = HandlePacket(pServerPacket);
					if (iRet != 0)
					{
						return;
					}

					if (m_iRecvBufLen > iFullLength)
					{
						memmove(m_RecvBuf, m_RecvBuf + iFullLength, m_iRecvBufLen - iFullLength);
					}
					m_iRecvBufLen -= iFullLength;
				}
				else
				{
					break;
				}
			}
		}
	} while (iLen);
}

void ClientBase::SendData(uint16 iTypeId, uint16 iLen, const char* pData)
{
	char* buf = m_pContextPool->PopOutputBuffer();
	ServerPacket* pPacket = (ServerPacket*)buf;
	pPacket->m_iLen = iLen;
	pPacket->m_iTypeId = iTypeId;

	memcpy(pPacket->m_Buf, pData, iLen);
	((Connection*)m_ConnId)->AsyncSend(pPacket->m_iLen + SERVER_PACKET_HEAD, buf);
}

int32 ClientBase::HandleLoginPacket(uint16 iLen, char *pBuf)
{
	m_iState = LOGGEDIN;

	if (m_bInLogin)
	{
		LoginClientSend::VersionReq(this, CLIENT_VERSION);
	}
	else
	{
		GateClientSend::AvatarListReq(this);
	}
	
	return 0;
}

int32 ClientBase::HandlePacket(ServerPacket* pPacket)
{
	Receiver::OnPacketReceived(this, pPacket->m_iTypeId, pPacket->m_iLen, pPacket->m_Buf);
	return 0;
}

void ClientBase::LoginNtf(uint32 iGateIP, uint16 iGatePort)
{
	// disconnect to login server
	Connection* pConnection = (Connection*)m_ConnId;
	pConnection->AsyncDisconnect();

	// record gate info
	m_iGateIP = iGateIP;
	m_iGatePort = iGatePort;
}

void ClientBase::AvatarListAck(int32 iRet, uint8 iAvatarCount, const ftdAvatar *arrayAvatar)
{

}





















bool CALLBACK ClientBase::OnConnection(ConnID connId)
{
	Connection* pConnection = (Connection*)connId;
	ClientBase* pClientBase = (ClientBase*)pConnection->GetClient();
	pClientBase->m_ConnId = connId;
	pClientBase->m_iState = CONNECTED;
	
	char* buf = pClientBase->m_pContextPool->PopOutputBuffer();
	memcpy(buf, (char*)&pClientBase->m_TokenPacket, MAX_INPUT_BUFFER);
	pConnection->AsyncSend(pClientBase->m_TokenPacket.m_iTokenLen + sizeof(uint16), buf);

	return true;
}

void CALLBACK ClientBase::OnDisconnect(ConnID connId)
{
	Connection* pConnection = (Connection*)connId;
	ClientBase* pClientBase = (ClientBase*)pConnection->GetClient();
	pClientBase->m_iState = DISCONNECTED;

	if (pClientBase->m_bInLogin)
	{
		pClientBase->m_iState = NOT_CONNECT;
		pClientBase->m_bInLogin = false;
		pClientBase->Login(pClientBase->m_iGateIP, pClientBase->m_iGatePort, pClientBase->m_TokenPacket.m_TokenBuf);
		
		return;
	}

	// notify UI
	if (pClientBase->m_pDisconnectEvent)
	{
		(*pClientBase->m_pDisconnectEvent)();
	}
}

void CALLBACK ClientBase::OnData(ConnID connId, uint32 iLen, char* pBuf)
{
	Connection* pConnection = (Connection*)connId;
	ClientBase* pClientBase = (ClientBase*)pConnection->GetClient();
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

int32 Sender::SendPacket(void* pClient, uint16 iTypeId, uint16 iLen, const char *pBuf)
{
	((ClientBase*)pClient)->SendData(iTypeId, iLen, pBuf);
	return 0;
}