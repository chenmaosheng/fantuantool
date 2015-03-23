#include "easy_client.h"
#include "easy_worker.h"
#include "easy_net.h"
#include "easy_connection.h"
#include "easy_handler.h"
#include "easy_logic_command.h"
#include "easy_client_loop.h"
#include "easy_event.h"
#include "easy_context.h"
#include "easy_packethandler.h"
#include "util.h"

EasyClient* g_pClient = NULL;

EasyClient::EasyClient()
{
	m_pWorker = NULL;
	Clear();
	InitializeCriticalSection(&m_csClientEvent);
}

EasyClient::~EasyClient()
{
	Clear();
	DeleteCriticalSection(&m_csClientEvent);
}

void EasyClient::Clear()
{
	m_ConnId = NULL;
	m_iRecvBufLen = 0;
	memset(m_RecvBuf, 0, sizeof(m_RecvBuf));
        
	m_ClientEventList.clear();

	m_iState = NOT_CONNECT;
}

int32 EasyClient::Init()
{
	int32 iRet = 0;

	InitNet();

	// create worker with 1 thread and context pool
	m_pWorker = CreateWorker(1);
        
	m_pMainLoop = new EasyClientLoop;
	m_pMainLoop->Init();
	m_pMainLoop->Start();

	return 0;
}

void EasyClient::Destroy()
{
	SAFE_DELETE(m_pMainLoop);
	DestroyNet();
}

void EasyClient::Login(uint32 iIP, uint16 iPort)
{
	LOG_DBG(LOG_SERVER, _T("Start to login"));
	if (m_ConnId)
	{
		CloseConnection((EasyConnection*)m_ConnId);
	}

	m_SockAddr.sin_family = AF_INET;
	m_SockAddr.sin_port = htons(iPort);
	m_SockAddr.sin_addr.s_addr = iIP;

	static EasyHandler handler;
	handler.OnConnection = &OnConnection;
	handler.OnDisconnect = &OnDisconnect;
	handler.OnData = &OnData;
	handler.OnConnectFailed = &OnConnectFailed;

	ConnectConnection(&m_SockAddr, &handler, m_pWorker, this);
}

void EasyClient::Logout()
{
	LOG_DBG(LOG_SERVER, _T("Start to logout"));
	if (m_ConnId)
	{
		AsyncDisconnect((EasyConnection*)m_ConnId);
	}
}

bool EasyClient::OnClientConnection(ConnID connId)
{
	m_ConnId = connId;
	m_iState = CONNECTED;

	LOG_DBG(LOG_SERVER, _T("Connect success"));
        
	return true;
}

void EasyClient::OnClientDisconnect(ConnID connId)
{
	m_iState = EasyClient::DISCONNECTED;

        
	// clear all states
	Clear();
}

void EasyClient::OnClientData(uint32 iLen, char* pBuf)
{
	uint32 iCopyLen = 0;
	int32 iRet = 0;
	// check if peer EasyClient is connected
	EasyConnection* pConnection = (EasyConnection*)m_ConnId;
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

		while (m_iRecvBufLen >= PACKET_HEAD)     // step2: check if buffer is larger than header
		{
			EasyPacket* pPacket = (EasyPacket*)m_RecvBuf;
			uint16 iFullLength = pPacket->m_iLen+PACKET_HEAD;
			if (m_iRecvBufLen >= iFullLength)       // step3: cut specific size from received buffer
			{
				iRet = HandlePacket(pPacket);
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
	} while (iLen);
}

void EasyClient::SendData(uint16 iTypeId, uint16 iLen, const char* pData)
{
	LOG_DBG(_T("Send Data, iTypeId=%d"), iTypeId);
	EasyContext* pContext = CreateContext(OPERATION_SEND);
	EasyPacket* pPacket = (EasyPacket*)pContext->buffer_;
	pPacket->m_iLen = iLen;
	pPacket->m_iTypeId = iTypeId;

	memcpy(pPacket->m_Buf, pData, iLen);
	AsyncRawSend((EasyConnection*)m_ConnId, pPacket->m_iLen + PACKET_HEAD, pContext->buffer_);
}


int32 EasyClient::HandlePacket(EasyPacket* pPacket)
{
	LOG_DBG(_T("receive packet, typeid=%d, len=%d"), pPacket->m_iTypeId, pPacket->m_iLen);
	PacketHandler::OnPacketReceived(this, pPacket->m_iTypeId, pPacket->m_iLen, pPacket->m_Buf);
	return 0;
}

EasyEvent* EasyClient::PopClientEvent()
{
	EasyEvent* pEvent = NULL;
	EnterCriticalSection(&m_csClientEvent);
	if (!m_ClientEventList.empty())
	{
		pEvent = m_ClientEventList.front();
		m_ClientEventList.pop_front();
	}
	LeaveCriticalSection(&m_csClientEvent);
	return pEvent;
}
















BOOL CALLBACK EasyClient::OnConnection(ConnID connId)
{
	EasyClient* pEasyClient = (EasyClient*)(((EasyConnection*)connId)->client_);
        
	LogicCommandOnConnect* pCommand = new LogicCommandOnConnect;
	pCommand->m_ConnId = connId;
	pEasyClient->m_pMainLoop->PushCommand(pCommand);
        
	return true;
}

void CALLBACK EasyClient::OnDisconnect(ConnID connId)
{
	EasyClient* pEasyClient = (EasyClient*)(((EasyConnection*)connId)->client_);

	LogicCommandOnDisconnect* pCommand = new LogicCommandOnDisconnect;
	pCommand->m_ConnId = connId;
	pEasyClient->m_pMainLoop->PushCommand(pCommand);
}

void CALLBACK EasyClient::OnData(ConnID connId, uint32 iLen, char* pBuf)
{
	EasyClient* pEasyClient = (EasyClient*)(((EasyConnection*)connId)->client_);
	LogicCommandOnData* pCommand = new LogicCommandOnData;
	LOG_DBG(_T("Len=%d"), iLen);
	pCommand->m_ConnId = connId;
	pCommand->CopyData(iLen, pBuf);
	pEasyClient->m_pMainLoop->PushCommand(pCommand);
}

void CALLBACK EasyClient::OnConnectFailed(void* pEasyClient)
{
	EasyClient* pSimpleEasyClient = (EasyClient*)pEasyClient;
	if (pSimpleEasyClient->m_iState == DESTORYING)
	{
		pSimpleEasyClient->m_iState = NOT_CONNECT;
	}
	else
	if (pSimpleEasyClient->m_iState == CONNECTING)
	{
		pSimpleEasyClient->m_iState = CONNECT_FAILED;
	}
}

int32 PacketHandler::SendPacket(void* pEasyClient, uint16 iTypeId, uint16 iLen, const char *pBuf)
{
	((EasyClient*)pEasyClient)->SendData(iTypeId, iLen, pBuf);
	return 0;
}