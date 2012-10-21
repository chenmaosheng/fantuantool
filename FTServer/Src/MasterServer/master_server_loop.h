/*****************************************************************************************
	filename:	master_server_loop.h
	created:	09/27/2012
	author:		chen
	purpose:	control game tick on master server

*****************************************************************************************/

#ifndef _H_MASTER_SERVER_LOOP
#define _H_MASTER_SERVER_LOOP

#include "logic_loop.h"
#include "object_pool.h"
#include "server_context.h"
#include <string>
#include <queue>

class MasterPlayerContext;
struct LogicCommandOnLoginReport;
struct LogicCommandOnGateReport;
struct LogicCommandOnLoginReq;
struct LogicCommandGateAllocAck;
struct LogicCommandOnGateLoginReq;
struct LogicCommandOnRegionAllocAck;
struct LogicCommandOnRegionLeaveReq;
struct LogicCommandOnSessionDisconnect;
struct LogicCommandPacketForward;
class MasterServerLoop : public LogicLoop
{
public:
	typedef LogicLoop super;

	// cstr
	MasterServerLoop();
	
	// initialize master server loop
	int32 Init();
	// destroy master server loop
	void Destroy();
	// start master server loop
	int32 Start();
	// check if is ready for shutdown
	bool IsReadyForShutdown() const;
	// change from login session to gate session
	void LoginSession2GateSession(MasterPlayerContext*, uint32 iLoginSessionId, uint32 iGateSessionId);

	// receive request about allocate a gate session
	int32 GateAllocReq();

	// shutdown one player in master server
	void ShutdownPlayer(MasterPlayerContext*);
	// add player to finalizing queue
	void AddPlayerToFinalizingQueue(MasterPlayerContext*);
	// totally delete a player
	void DeletePlayer(MasterPlayerContext*);
	// delete player from login servercontext
	void DeletePlayerFromLoginServerContext(MasterPlayerContext*);
	// delete player from gate context
	void DeletePlayerFromGateServerContext(MasterPlayerContext*);
	// add player to region context
	void AddPlayerToRegionServerContext(MasterPlayerContext*);
	// delete player from region context
	void DeletePlayerFromRegionServerContext(MasterPlayerContext*);
	// send channel info to client
	int32 SendChannelList(MasterPlayerContext*);
	// get channel id by channel name, failed return INVALID_CHANNEL_ID
	uint8 GetChannelId(const TCHAR* strChannelName);
	// get initial region server id
	uint8 GetInitialRegionServerId(uint8 iChannelId);

private:
	DWORD _Loop();
	
	bool _OnCommand(LogicCommand*);
	void _OnCommandShutdown();

private:
	// receive login server's report
	void _OnCommandOnLoginReport(LogicCommandOnLoginReport*);
	// receive gate server's report
	void _OnCommandOnGateReport(LogicCommandOnGateReport*);
	// receive request about login
	void _OnCommandOnLoginReq(LogicCommandOnLoginReq*);
	// acknowledge response about allocate a gate session
	void _OnCommandGateAllocAck(LogicCommandGateAllocAck*);
	// receive request about gate login
	void _OnCommandOnGateLoginReq(LogicCommandOnGateLoginReq*);
	// receive region alloc ack from region server
	void _OnCommandOnRegionAllocAck(LogicCommandOnRegionAllocAck*);
	// receive region leave req from region server
	void _OnCommandOnRegionLeaveReq(LogicCommandOnRegionLeaveReq*);
	// receive disconnect from session server
	void _OnCommandOnSessionDisconnect(LogicCommandOnSessionDisconnect*);
	// handle packet forwarding to other server
	void _OnCommandPacketForward(LogicCommandPacketForward*);

private:
	int32 m_iShutdownStatus;

	ObjectPool<MasterPlayerContext> m_PlayerContextPool;
	stdext::hash_map<std::wstring, MasterPlayerContext*> m_mPlayerContextByName;
	stdext::hash_map<uint32, MasterPlayerContext*> m_mPlayerContextBySessionId;

	LoginServerContext m_LoginServerContext;
	GateServerContext* m_arrayGateServerContext[SERVERCOUNT_MAX]; // gate server's context on master server
	ChannelContext* m_arrayChannelContext[CHANNEL_MAX]; // channel context on master server
	uint8 m_iChannelCount; // the count of channel

	std::queue<MasterPlayerContext*> m_PlayerFinalizingQueue;
};

#endif
