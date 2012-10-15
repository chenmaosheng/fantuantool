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
class RegionServerLoop : public LogicLoop
{
public:
	typedef LogicLoop super;

	// cstr
	RegionServerLoop();
	
	// initialize master server loop
	int32 Init();
	// destroy master server loop
	void Destroy();
	// start master server loop
	int32 Start();
	// check if is ready for shutdown
	bool IsReadyForShutdown() const;
	// shutdown one player in master server
	void ShutdownPlayer(RegionPlayerContext*);
	// add player to finalizing queue
	void AddPlayerToFinalizingQueue(RegionPlayerContext*);
	// totally delete a player
	void DeletePlayer(RegionPlayerContext*);

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
	void OnCommandOnRegionAllocReq(LogicCommandOnRegionAllocReq*);

private:
	int32 m_iShutdownStatus;
	uint16 m_iPlayerCount;
	uint16 m_iPlayerMax;

	ObjectPool<RegionPlayerContext> m_PlayerContextPool;
	stdext::hash_map<uint64, RegionPlayerContext*> m_mPlayerContextByAvatarId;

	GateServerContext* m_arrayGateServerContext[SERVERCOUNT_MAX]; // gate server's context on master server
	
	std::queue<RegionPlayerContext*> m_PlayerFinalizingQueue;
};

#endif
