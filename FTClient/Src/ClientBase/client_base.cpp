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
#include "client_command.h"
#include "client_loop.h"
#include "client_event.h"
#include "log_device.h"

#include "login_client_send.h"
#include "gate_client_send.h"

ClientConfig* g_pClientConfig = NULL;
ClientBase* g_pClientBase = NULL;

ClientBase::ClientBase()
{
	m_pWorker = NULL;
	m_pLogSystem = NULL;
	Clear();
	InitializeCriticalSection(&m_csClientEvent);
}

ClientBase::~ClientBase()
{
	Clear();
	DeleteCriticalSection(&m_csClientEvent);
}

void ClientBase::Clear()
{
	m_ConnId = NULL;
	m_bInLogin = true;
	m_iRecvBufLen = 0;
	memset(m_RecvBuf, 0, sizeof(m_RecvBuf));
	memset(&m_TokenPacket, 0, sizeof(m_TokenPacket));
	m_iGateIP = 0;
	m_iGatePort = 0;

	m_iAvatarId = 0;
	m_strAvatarName[0] = _T('\0');
	m_iLastChannelId = 0;

	m_ClientEventList.clear();

	m_iState = NOT_CONNECT;
}

int32 ClientBase::Init()
{
	int32 iRet = 0;

	// initialize minidump
	Minidump::Init(_T("log"));

	// set the min and max of memory pool object
	MEMORY_POOL_INIT(MEMORY_OBJECT_MIN, MEMORY_OBJECT_MAX);

	StarNet::Init();

	g_pClientConfig = new ClientConfig;
	g_pClientConfig->LoadConfig();

	// initialize log system
	m_pLogSystem = Log::GetInstance();
	m_pLogSystem->Init(g_pClientConfig->GetLogLevel());
	m_pLogSystem->SetLogTypeString(LOG_SERVER, _T("Client"));
	LogDevice* pDevice = NULL;
	pDevice = m_pLogSystem->CreateAndAddLogDevice(Log::LOG_DEVICE_DEBUG);
	pDevice = m_pLogSystem->CreateAndAddLogDevice(Log::LOG_DEVICE_FILE);
	if (pDevice)
	{
		pDevice->Init(g_pClientConfig->GetLogPath(), _T("Client"));
	}
	m_pLogSystem->Start();

	// create worker with 1 thread and context pool
	m_pWorker = Worker::CreateWorker(1);
	m_pContextPool = ContextPool::CreateContextPool(MAX_INPUT_BUFFER, MAX_OUTPUT_BUFFER);

	m_pMainLoop = new ClientLoop;
	m_pMainLoop->Init();
	m_pMainLoop->Start();

	return 0;
}

void ClientBase::Destroy()
{
	SAFE_DELETE(m_pMainLoop);
	SAFE_DELETE(g_pClientConfig);
	StarNet::Destroy();
}

void ClientBase::Login(uint32 iIP, uint16 iPort, const char *strToken)
{
	LOG_DBG(LOG_SERVER, _T("Start to login"));
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

void ClientBase::Logout()
{
	LOG_DBG(LOG_SERVER, _T("Start to logout"));
	if (m_ConnId)
	{
		((Connection*)m_ConnId)->AsyncDisconnect();
	}
}

void ClientBase::RequestCreateAvatar(const TCHAR* strAvatarName)
{
	ftdAvatarCreateData data;
	int32 iRet = 0;
	iRet = WChar2Char(strAvatarName, data.m_strAvatarName, AVATARNAME_MAX+1);
	if (iRet == 0)
	{
		return;
	}
	data.m_strAvatarName[iRet] = '\0';
	GateClientSend::AvatarCreateReq(this, data);
}

void ClientBase::RequestSelectAvatar(const TCHAR* strAvatarName)
{
	int32 iRet = 0;
	char strUtf8[AVATARNAME_MAX+1] = {0};
	iRet = WChar2Char(strAvatarName, strUtf8, AVATARNAME_MAX+1);
	if (iRet == 0)
	{
		return;
	}
	strUtf8[iRet] = '\0';
	GateClientSend::AvatarSelectReq(this, strUtf8);
}

void ClientBase::RequestSelectChannel(const TCHAR* strChannelName)
{
	int32 iRet = 0;
	char strUtf8[CHANNELNAME_MAX+1] = {0};
	iRet = WChar2Char(strChannelName, strUtf8, CHANNELNAME_MAX+1);
	if (iRet == 0)
	{
		return;
	}
	strUtf8[iRet] = '\0';
	GateClientSend::ChannelSelectReq(this, strUtf8);
}

void ClientBase::RequestLeaveChannel()
{
	GateClientSend::ChannelLeaveReq(this);
}

bool ClientBase::OnClientConnection(ConnID connId)
{
	m_ConnId = connId;
	m_iState = CONNECTED;

	LOG_DBG(LOG_SERVER, _T("Connect success"));

	char* buf = m_pContextPool->PopOutputBuffer();
	memcpy(buf, (char*)&m_TokenPacket, MAX_INPUT_BUFFER);
	((Connection*)m_ConnId)->AsyncSend(m_TokenPacket.m_iTokenLen + sizeof(uint16), buf);

	LOG_DBG(LOG_SERVER, _T("Send token"));
	
	return true;
}

void ClientBase::OnClientDisconnect(ConnID connId)
{
	m_iState = ClientBase::DISCONNECTED;

	if (m_bInLogin)
	{
		LOG_DBG(LOG_SERVER, _T("Finish disconnect on login server"));
		m_iState = ClientBase::NOT_CONNECT;
		m_bInLogin = false;
		Login(m_iGateIP, m_iGatePort, m_TokenPacket.m_TokenBuf);

		return;
	}

	LOG_DBG(LOG_SERVER, _T("Finish disconnect on gate server"));

	// clear all states
	Clear();

	ClientEventAvatarLogout* newEvent = FT_NEW(ClientEventAvatarLogout);
	EnterCriticalSection(&m_csClientEvent);	
	m_ClientEventList.push_back(newEvent);
	LeaveCriticalSection(&m_csClientEvent);
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
	LOG_DBG(LOG_SERVER, _T("Send Data, iTypeId=%d"), iTypeId);
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
		LOG_DBG(LOG_SERVER, _T("Receive login data on login server"));
		LoginClientSend::VersionReq(this, CLIENT_VERSION);
	}
	else
	{
		LOG_DBG(LOG_SERVER, _T("Receive login data on gate server"));
		GateClientSend::AvatarListReq(this);
	}
	
	return 0;
}

int32 ClientBase::HandlePacket(ServerPacket* pPacket)
{
	LOG_DBG(LOG_SERVER, _T("receive packet, typeid=%d, len=%d"), pPacket->m_iTypeId, pPacket->m_iLen);
	Receiver::OnPacketReceived(this, pPacket->m_iTypeId, pPacket->m_iLen, pPacket->m_Buf);
	return 0;
}

ClientEvent* ClientBase::PopClientEvent()
{
	ClientEvent* pEvent = NULL;
	EnterCriticalSection(&m_csClientEvent);
	if (!m_ClientEventList.empty())
	{
		pEvent = m_ClientEventList.front();
		m_ClientEventList.pop_front();
	}
	LeaveCriticalSection(&m_csClientEvent);
	return pEvent;
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

void ClientBase::AvatarListAck(int32 iReturn, uint8 iAvatarCount, const ftdAvatar *arrayAvatar)
{
	LOG_DBG(LOG_SERVER, _T("AvatarListAck"));

	ClientEventAvatarList* newEvent = FT_NEW(ClientEventAvatarList);
	newEvent->m_iReturn = iReturn;
	newEvent->m_iAvatarCount = iAvatarCount;
	memcpy(newEvent->m_Avatar, arrayAvatar, iAvatarCount*sizeof(ftdAvatar));
	EnterCriticalSection(&m_csClientEvent);
	m_ClientEventList.push_back(newEvent);
	LeaveCriticalSection(&m_csClientEvent);
}

void ClientBase::AvatarCreateAck(int32 iReturn, const ftdAvatar &newAvatar)
{
	LOG_DBG(LOG_SERVER, _T("AvatarCreateAck"));

	ClientEventAvatarCreate* newEvent = FT_NEW(ClientEventAvatarCreate);
	newEvent->m_iReturn = iReturn;
	memcpy(&newEvent->m_Avatar, &newAvatar, sizeof(ftdAvatar));
	EnterCriticalSection(&m_csClientEvent);
	m_ClientEventList.push_back(newEvent);
	LeaveCriticalSection(&m_csClientEvent);
}

void ClientBase::AvatarSelectAck(int32 iReturn, const ftdAvatarSelectData &data)
{
	LOG_DBG(LOG_SERVER, _T("AvatarSelectAck"));

	m_iAvatarId = data.m_iAvatarId;
	Char2WChar(data.m_strAvatarName, m_strAvatarName, AVATARNAME_MAX+1);
	m_iLastChannelId = data.m_iLastChannelId;
}

void ClientBase::ChannelListNtf(uint8 iChannelCount, const ftdChannelData* arrayChannelData)
{
	LOG_DBG(LOG_SERVER, _T("ChannelListNtf"));

	ClientEventChannelList* newEvent = FT_NEW(ClientEventChannelList);
	newEvent->m_iChannelCount = iChannelCount;
	memcpy(newEvent->m_arrayChannelData, arrayChannelData, sizeof(ftdChannelData)*iChannelCount);
	EnterCriticalSection(&m_csClientEvent);
	m_ClientEventList.push_back(newEvent);
	LeaveCriticalSection(&m_csClientEvent);
}




















bool CALLBACK ClientBase::OnConnection(ConnID connId)
{
	Connection* pConnection = (Connection*)connId;
	ClientBase* pClientBase = (ClientBase*)pConnection->GetClient();
	
	ClientCommandOnConnect* pCommand = FT_NEW(ClientCommandOnConnect);
	pCommand->m_ConnId = connId;
	pClientBase->m_pMainLoop->PushCommand(pCommand);
	
	return true;
}

void CALLBACK ClientBase::OnDisconnect(ConnID connId)
{
	Connection* pConnection = (Connection*)connId;
	ClientBase* pClientBase = (ClientBase*)pConnection->GetClient();

	ClientCommandOnDisconnect* pCommand = FT_NEW(ClientCommandOnDisconnect);
	pCommand->m_ConnId = connId;
	pClientBase->m_pMainLoop->PushCommand(pCommand);
}

void CALLBACK ClientBase::OnData(ConnID connId, uint32 iLen, char* pBuf)
{
	LOG_DBG(LOG_SERVER, _T("Len=%d"), iLen);
	Connection* pConnection = (Connection*)connId;
	ClientBase* pClientBase = (ClientBase*)pConnection->GetClient();
	ClientCommandOnData* pCommand = FT_NEW(ClientCommandOnData);
	pCommand->m_ConnId = connId;
	pCommand->CopyData(iLen, pBuf);
	pClientBase->m_pMainLoop->PushCommand(pCommand);
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