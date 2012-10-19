/*****************************************************************************************
	filename:	type.h
	created:	09/27/2012
	author:		chen
	purpose:	rename and define all necessary data types

*****************************************************************************************/

#ifndef _H_TYPE
#define _H_TYPE

#include <tchar.h>

typedef char				int8;
typedef unsigned char		uint8;
typedef short				int16;
typedef unsigned short		uint16;
typedef int					int32;
typedef unsigned int		uint32;
typedef __int64				int64;
typedef unsigned __int64	uint64;
typedef float				float32;
typedef double				float64;
typedef const char*			cpint8;

struct Vector3
{
	Vector3(float fX = 0.0f, float fY = 0.0f, float fZ = 0.0f) : m_fX(fX), m_fY(fY), m_fZ(fZ){}

	float m_fX;
	float m_fY;
	float m_fZ;
};

// log type define
enum
{
	LOG_NET,
	LOG_SERVER,
	LOG_PLAYER,
	LOG_DB,
	LOG_PROFILER,

	LOG_TYPE_MAX = LOG_DB + 1,
};

#endif
