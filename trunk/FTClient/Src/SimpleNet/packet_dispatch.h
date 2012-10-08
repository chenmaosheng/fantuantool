/*****************************************************************************************
filename:	packet_dispatch.h
created:	09/27/2012
author:		chen
purpose:	packet dispatcher, only for C/S

*****************************************************************************************/

#ifndef _H_PACKET_DISPATCH
#define _H_PACKET_DISPATCH

#include "data_stream.h"

#define DISPATCH_FILTER_MAX 255

// one filter of dispatcher, contains several functions
struct DispatchFilter
{
	typedef bool (CALLBACK* Func)(void*, InputStream&);
	uint32 m_iFuncCount;
	Func* m_pFunc;
};

// lot of filters of dispatcher
class DispatchFilterArray
{
public:
	static DispatchFilter& GetFilter(uint8 iFilterId);

private:
	static DispatchFilter m_arrDispatchFilter[DISPATCH_FILTER_MAX];
};

#endif
