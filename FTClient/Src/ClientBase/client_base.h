/*****************************************************************************************
	filename:	client_base.h
	created:	10/03/2012
	author:		chen
	purpose:	basic function of each client

*****************************************************************************************/

#ifndef _H_CLIENT_BASE
#define _H_CLIENT_BASE

#include "client_common.h"
#include "ftd_define.h"
#include "event.h"
#include "packet.h"
#include <list>
#include <hash_map>
#include "des.h"

struct TokenPacket
{
	DES_cblock des;
	uint16 m_iTokenLen;
	char m_TokenBuf[MAX_TOKEN_LEN];
};

class Worker;
class ContextPool;
struct Connector;
class ClientConfig;
class ClientLoop;
struct ClientCommand;
struct ClientEvent;
class Avatar;
class ClientBase
{
public:
	friend class ClientLoop;

	enum
	{
		NOT_CONNECT,
		CONNECTING,
		CONNECTED,
		CONNECT_FAILED,
		DISCONNECTED,
		DESTORYING,
		LOGGEDIN,
	};

	ClientBase();
	virtual ~ClientBase();
	void Clear();

	// initialize client
	virtual int32 Init();
	// destroy client
	virtual void Destroy();
	virtual bool OnClientConnection(ConnID connId);
	virtual void OnClientDisconnect(ConnID connId);
	// receive data
	virtual void OnClientData(uint32 iLen, char* pBuf);
	// send data
	virtual void SendData(uint16 iTypeId, uint16 iLen, const char* pData);
	// handle login packet which is analyzed from received buffer
	int32 HandleLoginPacket(uint16 iLen, char* pBuf);
	// handle server packet which is analyzed from received buffer
	int32 HandlePacket(ServerPacket*);
	// pop a event from event list;
	ClientEvent* PopClientEvent();

	// connect to login server
	void Login(uint32 iIP, uint16 iPort, const char* strToken);
	// disconnect
	void Logout();
	// request creating an avatar
	void RequestCreateAvatar(const TCHAR* strAvatarName);
	// request select an avatar
	void RequestSelectAvatar(const TCHAR* strAvatarName);
	// request select channel
	void RequestSelectChannel(const TCHAR* strChannelName);
	// request leave channel
	void RequestLeaveChannel();
	// send chat message
	void SendChatMessage(const TCHAR* strMessage);

public:
	// receive login ntf from master server
	void LoginNtf(uint32 iGateIP, uint16 iGatePort);
	// receive avatar list from db
	void AvatarListAck(int32 iReturn, uint8 iAvatarCount, const ftdAvatar *arrayAvatar);
	// receive avatar create
	void AvatarCreateAck(int32 iReturn, const ftdAvatar &newAvatar);
	// receive avatar select
	void AvatarSelectAck(int32 iReturn, const ftdAvatarSelectData &data);
	// receive channel list
	void ChannelListNtf(uint8 iChannelCount, const ftdChannelData* arrayChannelData);
	// receive channel select result
	void ChannelSelectAck(int32 iReturn);
	// receive server time
	void ServerTimeNtf(uint32 iServerTime);
	// receive avatar enter ntf
	void RegionAvatarEnterNtf(uint64 iAvatarId, const char* strAvatarName);
	// receive avatar leave ntf
	void RegionAvatarLeaveNtf(uint64 iAvatarId);
	// receive region chat
	void RegionChatNtf(uint64 iAvatarId, const char* strMessage);

private:
	// connection handler
	static bool CALLBACK OnConnection(ConnID connId);
	static void CALLBACK OnDisconnect(ConnID connId);
	static void CALLBACK OnData(ConnID connId, uint32 iLen, char* pBuf);
	static void CALLBACK OnConnectFailed(void*);

private:
	Worker* m_pWorker;
	ContextPool* m_pContextPool;
	Log* m_pLogSystem;
	ClientLoop* m_pMainLoop;

	ConnID m_ConnId;		// connection id
	SOCKADDR_IN m_SockAddr;
	int32 m_iState;	// current state about connect
	uint32 m_iRecvBufLen;	// already received buffer length
	char m_RecvBuf[MAX_OUTPUT_BUFFER];	// the whole received buffer
	bool m_bInLogin;

	TokenPacket m_TokenPacket;
	rsa_st* m_pRSA;
	DES_cblock m_Des;
	DES_key_schedule m_DesSchedule;

	uint32 m_iGateIP;
	uint16 m_iGatePort;

	// avatar info
	uint64 m_iAvatarId;
	TCHAR m_strAvatarName[AVATARNAME_MAX+1];
	uint8 m_iLastChannelId;
	uint32 m_iServerTime;

	stdext::hash_map<uint64, Avatar*> m_mAvatarListById;

	std::list<ClientEvent*> m_ClientEventList;
	CRITICAL_SECTION m_csClientEvent;
};

extern ClientConfig* g_pClientConfig;
extern ClientBase* g_pClientBase;

#endif
