#include "map.h"

float MapGrid::m_fGridSize = 0.0f;

MapGrid::MapGrid() : m_iGridX(0), m_iGridY(0), m_fMinPositionX(0.0f), m_fMaxPositionX(0.0f), m_fMinPositionY(0.0f), m_fMaxPositionY(0.0f)
{

}

MapGrid::~MapGrid()
{
	m_ActorList.clear();
}

void MapGrid::SetGridPosition(uint32 iGridX, uint32 iGridY)
{
	m_iGridX = iGridX;
	m_iGridY = iGridY;
}

void MapGrid::AddActor(Actor *pActor)
{
	m_ActorList.push_back(pActor);
}

void MapGrid::RemoveActor(Actor *pActor)
{
	std::list<Actor*>::iterator it = std::find(m_ActorList.begin(), m_ActorList.end(), pActor);
	if (it != m_ActorList.end())
	{
		m_ActorList.erase(it);
	}
}