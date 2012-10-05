#include "cache_logic_command.h"
#include "cache_server.h"
#include "cache_server_loop.h"

#include "cache_peer_recv.h"

#include "memory_object.h"

void CachePeerRecv::OnLoginReq(PEER_CLIENT pPeerClient, uint32 iSessionId, uint16 iAccountNameLen, const TCHAR *strAccountName)
{
	uint32 iRet = 0;
	LogicCommandOnLoginReq* pCommand = FT_NEW(LogicCommandOnLoginReq);
	if (!pCommand)
	{
		LOG_ERR(LOG_SERVER, _T("FT_NEW(LogicCommandOnLoginReq) failed"));
		return;
	}

	pCommand->m_iSessionId = iSessionId;
	wcscpy_s(pCommand->m_strAccountName, _countof(pCommand->m_strAccountName), strAccountName);
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}