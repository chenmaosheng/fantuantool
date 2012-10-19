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
	static float m_fGridSize;
	uint32 m_iGridX;
	uint32 m_iGridY;

	float m_fMinPositionX;
	float m_fMaxPositionX;
	float m_fMinPositionY;
	float m_fMaxPositionY;

	std::list<Actor*> m_ActorList;
};

class Map : public MemoryObject
{
public:
	// cstr and dstr
	Map();
	~Map();


};

#endif
