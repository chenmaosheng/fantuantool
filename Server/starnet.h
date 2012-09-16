#ifndef _H_STAR_NET
#define _H_STAR_NET

#include "common.h"

class StarNet
{
public:
	static int32 Init();
	static void Destroy();

	static LPFN_ACCEPTEX acceptex_;
	static LPFN_CONNECTEX connectex_;
	static LPFN_DISCONNECTEX disconnectex_;
};

#endif
