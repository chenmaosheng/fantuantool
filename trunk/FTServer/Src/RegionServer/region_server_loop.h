/*****************************************************************************************
	filename:	region_server_loop.h
	created:	10/14/2012
	author:		chen
	purpose:	control game tick on region server

*****************************************************************************************/

#ifndef _H_REGION_SERVER_LOOP
#define _H_REGION_SERVER_LOOP

#include "logic_loop.h"
#include "object_pool.h"
#include "server_helper.h"
#include <queue>
#include <hash_map>

class RegionPlayerContext;

// record all player contexts on one gate server
struct GateServerContext
{
	GateServerContext();
	~GateServerContext();

	RegionPlayerContext** m_arrayPlayerContext;
};

struct LogicCommandOnRegionAllocReq;
struct LogicCommandOnRegionReleaseReq;
struct LogicCommandOnRegionEnterReq;
struct LogicCommandOnRegionEnterAck;
struct LogicCommandOnRegionLeaveReq;
struct LogicCommandPacketForward;
class RegionServerLoop : public LogicLoop
{
public:
	typedef LogicLoop super;

	// cstr
	RegionServerLoop();
	
	// initialize region server loop
	int32 Init();
	// destroy region server loop
	void Destroy();
	// start region server loop
	int32 Start();
	// check if is ready for shutdown
	bool IsReadyForShutdown() const;
	// shutdown one player in region server
	void ShutdownPlayer(RegionPlayerContext*);
	// add player to finalizing queue
	void AddPlayerToFinalizingQueue(RegionPlayerContext*);
	// totally delete a player
	void DeletePlayer(RegionPlayerContext*);
	// get player by sessionid/avatarid
	RegionPlayerContext* GetPlayerContextBySessionId(uint32 iSessionId);
	RegionPlayerContext* GetPlayerContextByAvatarId(uint64 iAvatarId);

	// todo: temp function
	void BroadcastData(uint16 iTypeId, uint16 iLen, const char* pBuf);
	void SendRegionAvatars(RegionPlayerContext*);

private:
	DWORD _Loop();
	
	bool _OnCommand(LogicCommand*);
	void _OnCommandShutdown();
	// push player into gate context
	bool _PushPlayerToGateServerContext(uint32 iSessionId, RegionPlayerContext* pPlayerContext);
	// pop player from gate context
	void _PopPlayerFromGateServerContext(uint32 iSessionId);

private:
	// receive region alloc request
	void _OnCommandOnRegionAllocReq(LogicCommandOnRegionAllocReq*);
	// receive region release request
	void _OnCommandOnRegionReleaseReq(LogicCommandOnRegionReleaseReq*);
	// receive region enter request
	void _OnCommandOnRegionEnterReq(LogicCommandOnRegionEnterReq*);
	// receive region enter ack
	void _OnCommandOnRegionEnterAck(LogicCommandOnRegionEnterAck*);
	// receive region leave ack
	void _OnCommandOnRegionLeaveReq(LogicCommandOnRegionLeaveReq*);
	// handle packet forwarding to other server
	void _OnCommandPacketForward(LogicCommandPacketForward*);

private:
	int32 m_iShutdownStatus;
	uint16 m_iPlayerCount;
	uint16 m_iPlayerMax;

	ObjectPool<RegionPlayerContext> m_PlayerContextPool;
	stdext::hash_map<uint64, RegionPlayerContext*> m_mPlayerContextByAvatarId;

	GateServerContext* m_arrayGateServerContext[SERVERCOUNT_MAX]; // gate server's context on region server
	
	std::queue<RegionPlayerContext*> m_PlayerFinalizingQueue;

	BroadcastHelper m_BroadcastHelper;
};

#endif
