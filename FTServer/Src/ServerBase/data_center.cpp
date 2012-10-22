#include "data_center.h"
#include "map_desc.h"

DataCenter::DataCenter()
{
}

DataCenter::~DataCenter()
{
	Destroy();
}

int32 DataCenter::Init(const TCHAR *strPath)
{
	if (_LoadMapDesc(strPath) < 0)
	{
		return -1;
	}
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

int32 DataCenter::_LoadMapDesc(const TCHAR *strPath)
{
	return 0;
}