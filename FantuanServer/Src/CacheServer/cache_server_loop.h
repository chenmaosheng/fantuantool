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
class CacheServerLoop : public LogicLoop
{
public:
	typedef LogicLoop super;

	// cstr
	CacheServerLoop();

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

private:
	DWORD _Loop();

	bool _OnCommand(LogicCommand*);
	void _OnCommandShutdown();

private:
	// receive request about login
	void _OnCommandOnLoginReq(LogicCommandOnLoginReq*);

private:
	int32 m_iShutdownStatus;
	ObjectPool<CachePlayerContext> m_PlayerContextPool;
	stdext::hash_map<uint32, CachePlayerContext*> m_mPlayerContextBySessionId;
	std::queue<CachePlayerContext*> m_PlayerFinalizingQueue;

	DBConnPool* m_pDBConnPool;
};

#endif