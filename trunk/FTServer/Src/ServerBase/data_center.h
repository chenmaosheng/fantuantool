/*****************************************************************************************
	filename:	data_center.h
	created:	10/20/2012
	author:		chen
	purpose:	load and control all the data from xml

*****************************************************************************************/

#ifndef _H_DATA_CENTER
#define _H_DATA_CENTER

#include "server_common.h"
#include <vector>

struct MapDesc;
class DataCenter
{
public:
	// cstr and dstr
	DataCenter();
	~DataCenter();

	int32 Init(const TCHAR* strPath);
	void Destroy();

	MapDesc* GetMapDesc(uint32 iMapId);
	std::vector<MapDesc*>& GetAllMapDesc();

private:
	int32 _LoadMapDesc(const TCHAR* strPath);

private:
	std::vector<MapDesc*> m_vMapDesc;
};

#endif
