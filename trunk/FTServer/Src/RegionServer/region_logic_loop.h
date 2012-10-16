/*****************************************************************************************
	filename:	region_logic_loop.h
	created:	10/16/2012
	author:		chen
	purpose:	control part of game logic on region server

*****************************************************************************************/

#ifndef _H_REGION_LOGIC_LOOP
#define _H_REGION_LOGIC_LOOP

#include "logic_loop.h"
#include "object_pool.h"
#include <queue>
#include <hash_map>

class RegionPlayerContext;

class RegionLogicLoop : public LogicLoop
{
public:
	typedef LogicLoop super;

	// cstr
	RegionLogicLoop(uint32 iLoopId);
	
	// initialize region logic loop
	int32 Init(const std::vector<uint32>& vMapId, uint16 iInstanceCount);
	// destroy region logic loop
	void Destroy();
	// start region logic loop
	int32 Start();
	// check if is ready for shutdown
	bool IsReadyForShutdown() const;
	// shutdown one player in region server
	void ShutdownPlayer(RegionPlayerContext*);
	// add player to finalizing queue
	void AddPlayerToFinalizingQueue(RegionPlayerContext*);
	// totally delete a player
	void DeletePlayer(RegionPlayerContext*);

private:
	DWORD _Loop();
	
	bool _OnCommand(LogicCommand*);
	void _OnCommandShutdown();
	
private:
	
public:
	static RegionServerLoop* m_pMainLoop;
	uint32 m_iLoopId;

private:
	int32 m_iShutdownStatus;
	uint32 m_iPlayerCount;

	std::queue<RegionPlayerContext*> m_PlayerFinalizingQueue;
};

#endif
