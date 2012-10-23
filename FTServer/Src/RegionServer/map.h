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
#include <vector>

class Actor;
struct AreaDesc;
class MapGrid : public MemoryObject
{
public:
	// cstr and dstr
	MapGrid();
	~MapGrid();
	void SetGridPosition(uint32 iGridX, uint32 iGridY);
	void BindActor(Actor* pActor);
	void UnbindActor(Actor* pActor);

public:
	static float32 m_fGridSize;
	uint32 m_iGridX;
	uint32 m_iGridY;

	float32 m_fMinPositionX;
	float32 m_fMaxPositionX;
	float32 m_fMinPositionY;
	float32 m_fMaxPositionY;

	std::vector<AreaDesc*> m_arrayArea;
	std::list<Actor*> m_ActorList;
};

class RegionLogicLoop;
struct MapDesc;
class Map : public MemoryObject
{
public:
	// cstr and dstr
	Map();
	~Map();

	// initialize map
	int32 Init(uint16 iMapId);
	void Destroy();

	// bind actor to map
	int32 BindActor(Actor* pActor);
	// unbind actor on map
	void UnbindActor(Actor* pActor);
	// get grid by position
	MapGrid* GetGridByPosition(const Vector3& pos);

	static Map* Create(uint16 iMapId);
	static void Delete(Map*);

private:
	// allocate new actor id
	uint32 _AllocActorId(uint8 iActorType);
	// free actor id
	void _FreeActorId(uint32 iActorId);
	// test if two areas overlapped
	bool _CheckIfAreaOverlap(float fMinX1, float fMaxX1, float fMinY1, float fMaxY1, float fMinX2, float fMaxX2, float fMinY2, float fMaxY2);

public:
	uint16 m_iMapId;
	RegionLogicLoop* m_pRegionLogicLoop;

private:
	uint32 m_iActorMax;			// max count of actors on this map
	uint32 m_iActorIndexMax;	// the max value of actor index, different from m_iActorMax
	Actor** m_arrayActor;		// all actors
	std::list<uint32> m_ActorIdPool; // actor id allocator
	uint32 m_iAvatarCount;	// the count of avatars on this map

	MapDesc* m_pMapDesc;
	uint32 m_iHorizonGridCount;	// the count of grid horizontally
	uint32 m_iVerticalGridCount; // the count of grid vertically
	MapGrid*** m_array2dMapGrid;
};

#endif
