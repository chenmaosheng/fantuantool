#ifndef _H_PACKET_DISPATCH
#define _H_PACKET_DISPATCH

#include "data_stream.h"

#define DISPATCH_FILTER_MAX 255

struct DispatchFilter
{
	typedef bool (CALLBACK* Func)(void*, InputStream&);
	Func* m_pFunc;
};

class DispatchFilterArray
{
public:
	static DispatchFilter& GetFilter(uint8 iFilterId);

private:
	static DispatchFilter m_arrDispatchFilter[DISPATCH_FILTER_MAX];
};

#endif
