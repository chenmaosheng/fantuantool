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

void GateServerRecv::AvatarListAck(void *pClient, int32 iReturn, uint8 iAvatarCount, const ftdAvatar *arrayAvatar)
{
	ClientBase* pClientBase = (ClientBase*)pClient;
	pClientBase->AvatarListAck(iReturn, iAvatarCount, arrayAvatar);
}

void GateServerRecv::AvatarCreateAck(void *pClient, int32 iReturn, const ftdAvatar &newAvatar)
{
	ClientBase* pClientBase = (ClientBase*)pClient;
	pClientBase->AvatarCreateAck(iReturn, newAvatar);
}

void GateServerRecv::AvatarSelectAck(void *pClient, int32 iReturn, const ftdAvatarSelectData &data)
{
	ClientBase* pClientBase = (ClientBase*)pClient;
	pClientBase->AvatarSelectAck(iReturn, data);
}

void GateServerRecv::ChannelListNtf(void *pClient, uint8 iChannelCount, const ftdChannelData *arrayChannelData)
{
	ClientBase* pClientBase = (ClientBase*)pClient;
	pClientBase->ChannelListNtf(iChannelCount, arrayChannelData);
}

void GateServerRecv::ChannelSelectAck(void *pClient, int32 iReturn)
{
	ClientBase* pClientBase = (ClientBase*)pClient;
	pClientBase->ChannelSelectAck(iReturn);
}