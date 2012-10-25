#include "data_center.h"
#include "map_desc.h"
#include "tinyxml.h"

DataCenter::DataCenter()
{
}

DataCenter::~DataCenter()
{
	Destroy();
}

int32 DataCenter::Init(const char *strPath)
{
	if (_LoadMapDesc(strPath) < 0)
	{
		return -1;
	}

	return 0;
}

void DataCenter::Destroy()
{
	{
		std::vector<MapDesc*>::iterator it = m_vMapDesc.begin();
		while (it != m_vMapDesc.end())
		{
			SAFE_DELETE(*it);
			m_vMapDesc.erase(it);
			++it;
		}
	}
}

MapDesc* DataCenter::GetMapDesc(uint32 iMapId)
{
	std::vector<MapDesc*>::iterator it = m_vMapDesc.begin();
	while (it != m_vMapDesc.end())
	{
		if ((*it)->m_iMapId == iMapId)
		{
			return (*it);
		}

		++it;
	}

	return NULL;
}

std::vector<MapDesc*>& DataCenter::GetAllMapDesc()
{
	return m_vMapDesc;
}

int32 DataCenter::_LoadMapDesc(const char *strPath)
{
	int32 iRet = 0;
	char mapDescFile[MAX_PATH + 1] = {0};
	TiXmlDocument xmlDoc;

	// combine the filename and load xml file
	_snprintf_s(mapDescFile, MAX_PATH + 1, "%s\\MapDesc.xml", strPath);
	if (!xmlDoc.LoadFile(mapDescFile))
	{
		_ASSERT(false);
		return false;
	}

	TiXmlElement* pRootElement = xmlDoc.FirstChildElement("MapDescList");
	_ASSERT(pRootElement);
	if (!pRootElement)
	{
		return false;
	}

	for (TiXmlElement* pMapDescElement = pRootElement->FirstChildElement("MapDesc"); pMapDescElement != NULL; pMapDescElement = pMapDescElement->NextSiblingElement("MapDesc"))
	{
		// map id
		MapDesc* pMapDesc = new MapDesc;
		pMapDesc->m_iMapId = (uint16)atoi(pMapDescElement->Attribute("MapId"));

		// map name
		iRet = Char2WChar(pMapDescElement->Attribute("Name"), pMapDesc->m_strMapName, MAX_PATH + 1);
		_ASSERT(iRet);
		if (iRet == 0)
		{
			return false;
		}

		pMapDesc->m_strMapName[iRet] = _T('\0');
		
		// min x axis
		pMapDesc->m_fMinX = (float)atof(pMapDescElement->Attribute("MinX"));

		// max x axis
		pMapDesc->m_fMaxX = (float)atof(pMapDescElement->Attribute("MaxX"));

		// min y axis
		pMapDesc->m_fMinY = (float)atof(pMapDescElement->Attribute("MinY"));

		// max y axis
		pMapDesc->m_fMaxY = (float)atof(pMapDescElement->Attribute("MaxY"));

		// point list
		TiXmlElement* pPointListElement = pMapDescElement->FirstChildElement("PointDescList");
		_ASSERT(pPointListElement);
		if (!pPointListElement)
		{
			return false;
		}

		for (TiXmlElement* pPointElement = pPointListElement->FirstChildElement("PointDesc"); pPointElement != NULL; pPointElement = pPointElement->NextSiblingElement("PointDesc"))
		{
			PointDesc desc;
			// point id
			desc.m_iPointId = (uint16)atoi(pPointElement->Attribute("id"));

			// position
			desc.m_vPosition.m_fX = (float)atof(pPointElement->Attribute("PositionX"));
			desc.m_vPosition.m_fY = (float)atof(pPointElement->Attribute("PositionY"));
			desc.m_vPosition.m_fZ = (float)atof(pPointElement->Attribute("PositionZ"));

			// todo: rotation

			pMapDesc->m_vPointDesc.push_back(desc);
		}

		// area desc list
		TiXmlElement* pAreaListElement = pMapDescElement->FirstChildElement("AreaDescList");
		_ASSERT(pAreaListElement);
		if (!pAreaListElement)
		{
			return false;
		}
	
		for (TiXmlElement* pAreaElement = pAreaListElement->FirstChildElement("AreaDesc"); pAreaElement != NULL; pAreaElement = pAreaElement->NextSiblingElement("AreaDesc"))
		{
			AreaDesc desc;
			// point id
			desc.m_iAreaId = (uint16)atoi(pAreaElement->Attribute("id"));

			// min x axis
			desc.m_fMinX = (float)atof(pAreaElement->Attribute("MinX"));

			// max x axis
			desc.m_fMaxX = (float)atof(pAreaElement->Attribute("MaxX"));

			// min y axis
			desc.m_fMinY = (float)atof(pAreaElement->Attribute("MinY"));

			// max y axis
			desc.m_fMaxY = (float)atof(pAreaElement->Attribute("MaxY"));

			pMapDesc->m_vAreaDesc.push_back(desc);
		}
	}

	return 0;
}