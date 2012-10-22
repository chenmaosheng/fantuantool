/*****************************************************************************************
	filename:	map.h
	created:	10/19/2012
	author:		chen
	purpose:	define one map in the region

*****************************************************************************************/

#ifndef _H_MAP
#define _H_MAP

#include "server_common.h"
#include "memory_object.h"
#include <list>

class Actor;
class MapGrid : public MemoryObject
{
public:
	// cstr and dstr
	MapGrid();
	~MapGrid();
	void SetGridPosition(uint32 iGridX, uint32 iGridY);
	void AddActor(Actor* pActor);
	void RemoveActor(Actor* pActor);

public:
	static float32 m_fGridSize;
	uint32 m_iGridX;
	uint32 m_iGridY;

	float32 m_fMinPositionX;
	float32 m_fMaxPositionX;
	float32 m_fMinPositionY;
	float32 m_fMaxPositionY;

	std::list<Actor*> m_ActorList;
};

class RegionLogicLoop;
class Map : public MemoryObject
{
public:
	// cstr and dstr
	Map();
	~Map();

	// initialize map
	int32 Init(uint16 iMapId);
	void Destroy();

	static Map* Create(uint16 iMapId);
	static void Delete(Map*);

public:
	uint32 m_iMapId;
	RegionLogicLoop* m_pRegionLogicLoop;

private:
	uint32 m_iActorMax;			// max count of actors on this map
	Actor** m_arrayActor;		// all actors
	std::list<uint32> m_ActorIdPool; // actor id allocator
	uint32 m_iAvatarCount;	// the count of avatars on this map

	uint32 m_iHorizonGridCount;	// the count of grid horizontally
	uint32 m_iVerticalGridCount; // the count of grid vertically
	MapGrid*** m_array2dMapGrid;
};

#endif
