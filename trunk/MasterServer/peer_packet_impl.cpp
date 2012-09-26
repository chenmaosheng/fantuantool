#include "master_peer_recv.h"
#include "master_logic_command.h"
#include "master_server.h"
#include "master_server_loop.h"
#include "util.h"
#include "memory_object.h"

void MasterPeerRecv::OnLoginReq(PEER_CLIENT pPeerClient, uint32 iSessionId, const char* strAccountName)
{
	uint32 iRet = 0;
	LogicCommandOnLoginReq* pCommand = FT_NEW(LogicCommandOnLoginReq);
	pCommand->m_iSessionId = iSessionId;
	iRet = Char2WChar(strAccountName, pCommand->m_strAccountName, sizeof(pCommand->m_strAccountName)/sizeof(TCHAR));
	if (iRet == 0)
	{
		FT_DELETE(pCommand);
		return;
	}

	pCommand->m_strAccountName[iRet] = _T('\0');
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}

void MasterPeerRecv::GateHoldAck(PEER_CLIENT pPeerClient, uint16 iServerId, uint32 iLoginSessionId, const TCHAR *strAccountName, uint32 iGateSessionId)
{
	uint32 iRet = 0;
	LogicCommandGateHoldAck* pCommand = FT_NEW(LogicCommandGateHoldAck);
	pCommand->m_iServerId = iServerId;
	pCommand->m_iLoginSessionId = iLoginSessionId;
	pCommand->m_iGateSessionId = iGateSessionId;
	wcscpy_s(pCommand->m_strAccountName, sizeof(pCommand->m_strAccountName)/sizeof(TCHAR), strAccountName);
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}