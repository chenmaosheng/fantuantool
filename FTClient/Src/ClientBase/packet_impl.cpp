#include "login_server_recv.h"
#include "gate_server_recv.h"
#include "ftd_define.h"
#include "client_base.h"

void LoginServerRecv::LoginFailedAck(void* pClient, int32 iReason)
{

}

void LoginServerRecv::LoginNtf(void* pClient, uint32 iGateIP, uint16 iGatePort)
{
	ClientBase* pClientBase = (ClientBase*)pClient;
	pClientBase->LoginNtf(iGateIP, iGatePort);
}

void LoginServerRecv::VersionAck(void* pClient, int32 iReason)
{

}

void GateServerRecv::AvatarListAck(void *pClient, int32 iRet, uint8 iAvatarCount, const ftdAvatar *arrayAvatar)
{
	ClientBase* pClientBase = (ClientBase*)pClient;
	pClientBase->AvatarListAck(iRet, iAvatarCount, arrayAvatar);
}