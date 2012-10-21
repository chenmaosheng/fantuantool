/*****************************************************************************************
	filename:	cache_server_loop.h
	created:	10/05/2012
	author:		chen
	purpose:	control game tick on cache server

*****************************************************************************************/

#ifndef _H_CACHE_SERVER_LOOP
#define _H_CACHE_SERVER_LOOP

#include "logic_loop.h"
#include "object_pool.h"
#include <queue>
#include <hash_map>

class DBConnPool;
class CachePlayerContext;
struct LogicCommandOnLoginReq;
struct LogicCommandOnLogoutReq;
struct LogicCommandOnRegionEnterReq;
struct LogicCommandPacketForward;
class DBEvent;
class PlayerDBEvent;
class CacheServerLoop : public LogicLoop
{
public:
	typedef LogicLoop super;

	// cstr and dstr
	CacheServerLoop();
	~CacheServerLoop();

	// initialize cache server loop
	int32 Init();
	// destroy cache server loop
	void Destroy();
	// start cache server loop
	int32 Start();
	// Stop cache server loop
	void Stop();
	// check if is ready for shutdown
	bool IsReadyForShutdown() const;
	// shutdown one player in cache server
	void ShutdownPlayer(CachePlayerContext*);
	// add player to finalizing queue
	void AddPlayerToFinalizingQueue(CachePlayerContext*);
	// totally delete a player
	void DeletePlayer(CachePlayerContext*);
	// get player context by avatar id
	CachePlayerContext* GetPlayerContextByAvatarId(uint64 iAvatarId);
	// add player context by avatar id
	void AddPlayerContextByAvatarId(CachePlayerContext*);

private:
	DWORD _Loop();
	
	bool _OnCommand(LogicCommand*);
	void _OnCommandShutdown();

private:
	// receive request about login
	void _OnCommandOnLoginReq(LogicCommandOnLoginReq*);
	// receive logout request
	void _OnCommandOnLogoutReq(LogicCommandOnLogoutReq*);
	// receive region enter request
	void _OnCommandOnRegionEnterReq(LogicCommandOnRegionEnterReq*);
	// handle packet forwarding to other server
	void _OnCommandPacketForward(LogicCommandPacketForward*);

private:
	// get result from result queue
	void _OnDBEventResult(DBEvent* pEvent);
	void _OnPlayerEventResult(PlayerDBEvent* pEvent);

	// report own state to master server
	void _ReportState();
	// save data to db
	void _SaveDataToDB();
	
private:
	int32 m_iShutdownStatus;
	ObjectPool<CachePlayerContext> m_PlayerContextPool;
	stdext::hash_map<uint32, CachePlayerContext*> m_mPlayerContextBySessionId;
	stdext::hash_map<uint64, CachePlayerContext*> m_mPlayerContextByAvatarId;
	std::queue<CachePlayerContext*> m_PlayerFinalizingQueue;

public:
	DBConnPool* m_pDBConnPool;
};

#endif