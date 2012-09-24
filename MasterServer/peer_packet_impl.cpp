#include "master_peer_recv.h"
#include "master_logic_command.h"
#include "master_server.h"
#include "master_server_loop.h"

void MasterPeerRecv::OnLoginReq(PEER_CLIENT pPeerClient, uint32 iSessionId, const char* strAccountName)
{
	uint32 iRet = 0;
	LogicCommandOnLoginReq* pCommand = new LogicCommandOnLoginReq;
	pCommand->m_iSessionId = iSessionId;
	iRet = MultiByteToWideChar(CP_UTF8, 0, strAccountName, strlen(strAccountName)+1, pCommand->m_strAccountName, sizeof(pCommand->m_strAccountName)/sizeof(TCHAR));
	pCommand->m_strAccountName[iRet] = '\0';

	g_pServer->m_pMainLoop->PushCommand(pCommand);
}
