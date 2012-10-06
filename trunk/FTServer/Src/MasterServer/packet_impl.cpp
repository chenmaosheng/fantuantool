#include "master_player_context.h"
#include "gate_client_recv.h"

void GateClientRecv::AvatarListReq(void* pClient)
{
	MasterPlayerContext* pPlayerContext = (MasterPlayerContext*)pClient;
	pPlayerContext->OnAvatarListReq();
}
