/*****************************************************************************************
	filename:	map_desc.h
	created:	10/20/2012
	author:		chen
	purpose:	map description

*****************************************************************************************/

#ifndef _H_MAP_DESC
#define _H_MAP_DESC

#include "server_common.h"

struct PointDesc
{
	PointDesc() : m_iPointId(0)
	{
	}

	uint16 m_iPointId;
	Vector3 m_vPosition;
	Vector3 m_vRotation;
};

struct AreaDesc
{
	AreaDesc() : m_iAreaId(0), m_fMinX(0.0f), m_fMaxX(0.0f), m_fMinY(0.0f), m_fMaxY(0.0f)
	{
	}

	uint32 m_iAreaId;
	float32 m_fMinX;	// left
	float32 m_fMaxX;	// right
	float32 m_fMinY;	// top
	float32 m_fMaxY;	// bottom
};

struct MapDesc
{
	MapDesc() : m_iMapId(0), m_fMinX(0.0f), m_fMaxX(0.0f), m_fMinY(0.0f), m_fMaxY(0.0f)
	{
	}

	uint32 m_iMapId;
	std::wstring m_strMapName;
	float32 m_fMinX;	// left
	float32 m_fMaxX;	// right
	float32 m_fMinY;	// top
	float32 m_fMaxY;	// bottom
	
	std::vector<PointDesc> m_vPointDesc;
	std::vector<AreaDesc> m_vAreaDesc;
};

#endif
