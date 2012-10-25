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
#include "server_helper.h"
#include <queue>
#include <hash_map>

class RegionPlayerContext;
class Map;
class RegionServerLoop;
class Avatar;
struct LogicCommandMapEnterReq;
class RegionLogicLoop : public LogicLoop
{
public:
	typedef LogicLoop super;

	// cstr
	RegionLogicLoop(uint32 iLoopId);
	~RegionLogicLoop();
	
	// initialize region logic loop
	int32 Init(const std::vector<uint16>& vMapId, uint16 iInstanceCount);
	// destroy region logic loop
	void Destroy();
	// start region logic loop
	int32 Start();
	// check if is ready for shutdown
	bool IsReadyForShutdown() const;
	// get delay data
	DelaySendData& GetDelaySendData()
	{
		return m_DelaySendData;
	}
	// get broadcast helper
	BroadcastHelper& GetBroadcastHelper()
	{
		return m_BroadcastHelper;
	}


	// shutdown one player in region server
	void ShutdownPlayer(RegionPlayerContext*);
	// add player to finalizing queue
	void AddPlayerToFinalizingQueue(RegionPlayerContext*);
	// totally delete a player
	void DeletePlayer(RegionPlayerContext*);
	// get map
	Map* GetMapById(uint16 iMapId);

	// map enter
	void PushMapEnterCommand(RegionPlayerContext*, uint16 iMapId);

private:
	DWORD _Loop();
	
	bool _OnCommand(LogicCommand*);
	void _OnCommandShutdown();
	
	void _OnCommandMapEnterReq(LogicCommandMapEnterReq*);

private:
	// map related
	stdext::hash_map<uint16, Map*> m_mMapById;

public:
	static RegionServerLoop* m_pMainLoop;
	uint32 m_iLoopId;

private:
	int32 m_iShutdownStatus;
	uint32 m_iPlayerCount;
	BroadcastHelper m_BroadcastHelper;
	DelaySendData m_DelaySendData;

	Avatar** m_arrayAvatarHelper;
	std::queue<RegionPlayerContext*> m_PlayerFinalizingQueue;
};

#endif
