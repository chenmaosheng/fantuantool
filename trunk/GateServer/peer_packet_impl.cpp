#include "gate_peer_recv.h"
#include "gate_logic_command.h"
#include "gate_server.h"
#include "gate_server_loop.h"
#include "util.h"
#include "memory_object.h"

void GatePeerRecv::GateHoldReq(PEER_CLIENT pPeerClient, uint32 iLoginSessionId, const TCHAR* strAccountName)
{
	uint32 iRet = 0;
	LogicCommandGateHoldReq* pCommand = FT_NEW(LogicCommandGateHoldReq);
	pCommand->m_iLoginSessionId = iLoginSessionId;
	wcscpy_s(pCommand->m_strAccountName, sizeof(pCommand->m_strAccountName)/sizeof(TCHAR), strAccountName);
	g_pServer->m_pMainLoop->PushCommand(pCommand);
}
