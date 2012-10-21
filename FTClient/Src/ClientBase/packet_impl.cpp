#include "login_server_recv.h"
#include "gate_server_recv.h"
#include "region_server_recv.h"
#include "ftd_define.h"
#include "client_base.h"

void LoginServerRecv::LoginFailedAck(void* pClient, int32 iReason)
{
	ClientBase* pClientBase = (ClientBase*)pClient;
	pClientBase->LoginFailedAck(iReason);
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

void GateServerRecv::ChannelLeaveAck(void* pClient)
{
}

void RegionServerRecv::ServerTimeNtf(void *pClient, uint32 iServerTime)
{
	ClientBase* pClientBase = (ClientBase*)pClient;
	pClientBase->ServerTimeNtf(iServerTime);
}

void RegionServerRecv::InitialAvatarDataNtf(void* pClient, uint64 iAvatarId, const char* strAvatarName)
{

}

void RegionServerRecv::RegionAvatarEnterNtf(void* pClient, uint64 iAvatarId, const char* strAvatarName)
{
	ClientBase* pClientBase = (ClientBase*)pClient;
	pClientBase->RegionAvatarEnterNtf(iAvatarId, strAvatarName);
}

void RegionServerRecv::RegionAvatarLeaveNtf(void* pClient, uint64 iAvatarId)
{
	ClientBase* pClientBase = (ClientBase*)pClient;
	pClientBase->RegionAvatarLeaveNtf(iAvatarId);
}

void RegionServerRecv::RegionChatNtf(void* pClient, uint64 iAvatarId, const char* strMessage)
{
	ClientBase* pClientBase = (ClientBase*)pClient;
	pClientBase->RegionChatNtf(iAvatarId, strMessage);
}