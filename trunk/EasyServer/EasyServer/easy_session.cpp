#include "easy_session.h"
#include "dispatcher.h"
#include "easy_server_message.h"

EasyServerLoop* EasySession::m_pMainLoop = NULL;

void EasySession::OnPingReq(uint32 iVersion)
{
	LOG_STT(_T("OnPingReq"));
	PingAck(this, iVersion);
}



int32 Sender::SendPacket(void* pClient, uint16 iTypeId, uint16 iLen, const char* pBuf)
{
	return ((EasySession*)pClient)->SendData(iTypeId, iLen, pBuf);
}