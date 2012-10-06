#include "session.h"
#include "server_base.h"
#include "connection.h"
#include "packet.h"
#include "context_pool.h"
#include "single_buffer.h"
#include "data_stream.h"
#include "logic_command.h"
#include "logic_loop.h"

ServerBase* Session::m_pServer = NULL;
char* Session::m_pPrivateKey = NULL;
uint16 Session::m_iPrivateKeyLen = 0;

void Session::Initialize(const TCHAR* strPrivateKeyFile, ServerBase* pServer)
{
	m_pServer = pServer;
	// todo: fake secret key
	m_pPrivateKey = _strdup("private key");
	m_iPrivateKeyLen = (uint16)strlen(m_pPrivateKey);
}

Session::Session() :
m_StateMachine(SESSION_STATE_NONE) // reset state machine
{
	m_iSessionId = 0;
	m_pConnection = NULL;
	m_iRecvBufLen = 0;
	m_RecvBuf[0] = '\0';
	m_dwConnectionTime = 0;
	m_dwLoggedInTime = 0;
	m_iLoginBufLen = 0;
	memset(&m_TokenPacket, 0, sizeof(m_TokenPacket));
}

Session::~Session()
{
	m_pConnection = NULL;
}

void Session::Clear()
{
	m_pConnection = NULL;
	m_iRecvBufLen = 0;
	m_RecvBuf[0] = '\0';
	m_dwConnectionTime = 0;
	m_dwLoggedInTime = 0;
	m_iLoginBufLen = 0;
	memset(&m_TokenPacket, 0, sizeof(m_TokenPacket));
	m_StateMachine.SetCurrState(SESSION_STATE_NONE);
}

int32 Session::OnConnection(ConnID connId)
{
	int32 iRet = 0;

	// check state
	if (m_StateMachine.StateTransition(SESSION_EVENT_ONCONNECTION, false) != SESSION_STATE_ONCONNECTION)
	{
		LOG_ERR(LOG_SERVER, _T("Session state error, state=%d"), m_StateMachine.GetCurrState());
		return -1;
	}

	int32 i = sizeof(SessionId);

	// reset sequence
	((SessionId*)(&m_iSessionId))->sValue_.sequence_++;

	// coordinate conn and session
	m_pConnection = (Connection*)connId;

	m_pConnection->SetClient(this);
	m_pConnection->SetRefMax(256);

	// set new state
	iRet = m_StateMachine.StateTransition(SESSION_EVENT_ONCONNECTION);
	if (iRet < 0)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x Set new state failed"), m_iSessionId);
		return -1;
	}

	// set session's onconnection time
	m_dwConnectionTime = m_pServer->GetCurrTime();
	return 0;
}

void Session::OnDisconnect()
{
	// check and set state
	if (m_StateMachine.StateTransition(SESSION_EVENT_ONDISCONNECT) != SESSION_STATE_ONDISCONNECT)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x Set new state failed"), m_iSessionId);
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

void Session::OnData(uint16 iLen, char* pBuf)
{
	uint16 iCopyLen = 0;
	int32 iRet = 0;

	// check state
	if (m_StateMachine.StateTransition(SESSION_EVENT_ONDATA, false) != m_StateMachine.GetCurrState())
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x, Session state error, state=%d"), m_iSessionId, m_StateMachine.GetCurrState());
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
			while (m_iRecvBufLen >= m_iPrivateKeyLen)
			{
				iRet = HandleLoginPacket(m_iRecvBufLen, m_RecvBuf);
				if (iRet < 0)
				{
					return;
				}

				if (m_iRecvBufLen > m_iPrivateKeyLen)
				{
					memmove(m_RecvBuf, m_RecvBuf + m_iPrivateKeyLen, m_iRecvBufLen - m_iPrivateKeyLen);
				}
				m_iRecvBufLen -= m_iPrivateKeyLen;

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
			while (m_iRecvBufLen > SERVER_PACKET_HEAD)	// step2: check if buffer is larger than header
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

void Session::Disconnect()
{
	if (m_pConnection)
	{
		Connection::Close(m_pConnection);
	}
	else
	{
		OnDisconnect();
	}
}

int32 Session::SendData(uint16 iTypeId, uint16 len, const char *data)
{
	char* buf = m_pServer->GetContextPool()->PopOutputBuffer();
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

int32 Session::HandlePacket(ServerPacket* pPacket)
{
	Receiver::OnPacketReceived(this, pPacket->m_iTypeId, pPacket->m_iLen, pPacket->m_Buf);
	return 0;
}

int32 Session::HandleLoginPacket(uint16 iLen, char *pBuf)
{
	int32 iRet = 0;

	// todo: decrypt by private key, now is fake
	iRet = iLen;
	memcpy((char*)&m_TokenPacket+m_iLoginBufLen, pBuf, iLen);

	m_iLoginBufLen += iLen;

	if (m_iLoginBufLen < sizeof(uint16))
	{
		// continue receiving
		return 0;
	}

	// check if token buffer is larger than normal
	if (m_iLoginBufLen > m_TokenPacket.m_iTokenLen + sizeof(uint16) ||
		m_TokenPacket.m_iTokenLen > MAX_TOKEN_LEN)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x token data error"), m_iSessionId);
		Disconnect();
		return -1;
	}

	if (m_TokenPacket.m_iTokenLen + sizeof(uint16) == m_iLoginBufLen)
	{
		// check token packet is valid
		iRet = CheckLoginToken(m_TokenPacket.m_iTokenLen, m_TokenPacket.m_TokenBuf);
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("sid=%08x token data is invalid"), m_iSessionId);
			Disconnect();
			return -2;
		}

		Connection* pConnection = m_pConnection;

		// notify client login success
		iRet = LoggedInNtf();
		if (iRet != 0)
		{
			LOG_ERR(LOG_SERVER, _T("sid=%08x LoggedInNtf failed"), m_iSessionId);
			Disconnect();
			return -3;
		}

		char* strLoggedInNtf = (char*)pConnection->context_pool_->PopOutputBuffer();
		if (!strLoggedInNtf)
		{
			LOG_ERR(LOG_SERVER, _T("sid=%08x PopOutputBuffer failed"), m_iSessionId);
			Disconnect();
			return -4;
		}

		strcpy_s(strLoggedInNtf, MAX_OUTPUT_BUFFER, g_LoggedInNtf);

		pConnection->AsyncSend(strlen(strLoggedInNtf), (char*)strLoggedInNtf);

		LOG_DBG(LOG_SERVER, _T("sid=%08x LoggedIn success"), m_iSessionId);

		return 1;	// success
	}
	else
	{
		// continue receiving
		return 0;
	}
}

int32 Session::LoggedInNtf()
{
	// check state
	if (m_StateMachine.StateTransition(SESSION_EVENT_LOGGEDIN) != SESSION_STATE_LOGGEDIN)
	{
		LOG_ERR(LOG_SERVER, _T("sid=%08x Session state error, state=%d"), m_iSessionId, m_StateMachine.GetCurrState());
		return -1;
	}
	
	m_dwLoggedInTime = m_pServer->GetCurrTime();

	LOG_DBG(LOG_SERVER, _T("sid=%08x send login success notification to client"), m_iSessionId);

	return 0;
}