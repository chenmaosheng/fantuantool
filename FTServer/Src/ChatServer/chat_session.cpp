#include "chat_session.h"
#include "server_base.h"
#include "connection.h"
#include "packet.h"
#include "context_pool.h"
#include "single_buffer.h"
#include "data_stream.h"
#include "logic_command.h"
#include "logic_loop.h"

ChatServerLoop* ChatSession::m_pMainLoop = NULL;
ServerBase* ChatSession::m_pServer = NULL;

int32 ChatSession::Initialize(ServerBase* pServer)
{
	m_pServer = pServer;
	return 0;
}

ChatSession::ChatSession() :
m_StateMachine(SESSION_STATE_NONE) // reset state machine
{
	m_iSessionId = 0;
	m_pConnection = NULL;
	m_iRecvBufLen = 0;
	m_RecvBuf[0] = '\0';
	m_dwConnectionTime = 0;
	m_dwLoggedInTime = 0;
	m_iLoginBufLen = 0;

	// initialize state machine
	InitStateMachine();
}

ChatSession::~ChatSession()
{
	Clear();
	m_pConnection = NULL;
}

void ChatSession::Clear()
{
	m_pConnection = NULL;
	m_iRecvBufLen = 0;
	m_RecvBuf[0] = '\0';
	m_dwConnectionTime = 0;
	m_dwLoggedInTime = 0;
	m_iLoginBufLen = 0;
	m_StateMachine.SetCurrState(SESSION_STATE_NONE);
}

int32 ChatSession::OnConnection(ConnID connId)
{
	int32 iRet = 0;

	// check state
	if (m_StateMachine.StateTransition(SESSION_EVENT_ONCONNECTION, false) != SESSION_STATE_ONCONNECTION)
	{
		LOG_ERR(LOG_SERVER, _T("ChatSession state error, state=%d"), m_StateMachine.GetCurrState());
		_ASSERT(false && _T("state error"));
		return -1;
	}

	// reset sequence
	((SessionId*)(&m_iSessionId))->sValue_.sequence_++;

	// coordinate conn and session
	m_pConnection = (Connection*)connId;

	m_pConnection->SetClient(this);
	m_pConnection->SetRefMax(256);

	// set new state
	if (m_StateMachine.StateTransition(SESSION_EVENT_ONCONNECTION) < 0)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x state=%d state error"), m_iSessionId, m_StateMachine.GetCurrState());
		_ASSERT(false && _T("state error"));
		return -1;
	}

	// set session's onconnection time
	m_dwConnectionTime = m_pServer->GetCurrTime();
	return 0;
}

void ChatSession::OnDisconnect()
{
	// check and set state
	if (m_StateMachine.StateTransition(SESSION_EVENT_ONDISCONNECT) != SESSION_STATE_ONDISCONNECT)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x state=%d state error"), m_iSessionId, m_StateMachine.GetCurrState());
		_ASSERT(false && _T("state error"));
		return;
	}

	if (m_pConnection)
	{
		Connection::Close(m_pConnection);
		m_pConnection = NULL;
	}

	// reset sequence
	((SessionId*)(&m_iSessionId))->sValue_.sequence_++;

	Clear();
}

void ChatSession::OnData(uint16 iLen, char* pBuf)
{
	uint16 iCopyLen = 0;
	int32 iRet = 0;

	// check state
	if (m_StateMachine.StateTransition(SESSION_EVENT_ONDATA, false) != m_StateMachine.GetCurrState())
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x, ChatSession state error, state=%d"), m_iSessionId, m_StateMachine.GetCurrState());
		_ASSERT(false && _T("state error"));
		return;
	}

	do
	{
		// the incoming length is no more than the last of buffer
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
		}	// step1: received a raw buffer

		// if clinet is just connected
		if (m_StateMachine.GetCurrState() == SESSION_STATE_ONCONNECTION)
		{
			while (m_iRecvBufLen > 0)
			{
				iRet = HandleLoginPacket(m_iRecvBufLen, m_RecvBuf);
				if (iRet != 0)
				{
					return;
				}

				
				m_iRecvBufLen = 0;
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
	}while (iLen);
}

void ChatSession::Disconnect()
{
	if (m_pConnection)
	{
		m_pConnection->AsyncDisconnect();
	}
	else
	{
		OnDisconnect();
	}
}

int32 ChatSession::SendData(uint16 iTypeId, uint16 len, const char *data)
{
	char* buf = m_pServer->GetContextPool()->PopOutputBuffer();
	_ASSERT(buf);
	if (!buf)
	{
		return -1;
	}

	ServerPacket* pPacket = (ServerPacket*)buf;
	pPacket->m_iLen = len;
	pPacket->m_iTypeId = iTypeId;

	memcpy(pPacket->m_Buf, data, len);
	m_pConnection->AsyncSend(pPacket->m_iLen + SERVER_PACKET_HEAD, buf);

	return 0;
}

int32 ChatSession::HandlePacket(ServerPacket* pPacket)
{
	Receiver::OnPacketReceived(this, pPacket->m_iTypeId, pPacket->m_iLen, pPacket->m_Buf);
	return 0;
}

int32 ChatSession::HandleLoginPacket(uint16 iLen, char* pBuf)
{
	int32 iRet = 0;

	// check token packet is valid
	iRet = CheckLoginToken(iLen, pBuf);
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x token data is invalid"), m_iSessionId);
		Disconnect();
		return -2;
	}

	// notify client login success
	iRet = LoggedInNtf();
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x LoggedInNtf failed"), m_iSessionId);
		Disconnect();
		return -3;
	}

	LOG_DBG(LOG_SERVER, _T("sid=%08x LoggedIn success"), m_iSessionId);

	return 1;	// success
}

int32 ChatSession::CheckLoginToken(uint16 iLen, char* pBuf)
{
	int32 iRet = 0;
	// todo: pBuf's format is (Account;)
	char strAccountName[ACCOUNTNAME_MAX*3 + 1] = {0};
	strcpy_s(strAccountName, iLen, pBuf);

	iRet = Char2WChar(strAccountName, m_strAccountName, _countof(m_strAccountName));
	if (iRet == 0)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x Char2WChar error"), m_iSessionId);
		return -1;
	}

	// check if accountName is empty
	if (wcslen(m_strAccountName) == 0)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x m_strAccountName is empty"), m_iSessionId);
		return -1;
	}

	// change accountName to lowercase
	LowerCase(m_strAccountName);

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x Token is valid"), m_strAccountName, m_iSessionId);

	return 0;
}

int32 ChatSession::LoggedInNtf()
{
	int32 iRet = 0;
	// check state
	if (m_StateMachine.StateTransition(SESSION_EVENT_LOGGEDIN) != SESSION_STATE_LOGGEDIN)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x Session state error, state=%d"), m_iSessionId, m_StateMachine.GetCurrState());
		_ASSERT(false && _T("state error"));
		return -1;
	}
	
	m_dwLoggedInTime = m_pServer->GetCurrTime();

	LOG_DBG(LOG_SERVER, _T("sid=%08x send login success notification to client"), m_iSessionId);

	LOG_DBG(LOG_SERVER, _T("acc=%s sid=%08x loggin ntf send success"), m_strAccountName, m_iSessionId);
	return 0;
}

int32 Sender::SendPacket(void* pClient, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	return ((ChatSession*)pClient)->SendData(iTypeId, iLen, pBuf);
}
