#ifndef _H_NETWORK
#define _H_NETWORK

#include "common.h"



class Network
{
public:
	// intialize starnet
	static int32 Init();
	// destroy starnet
	static void Destroy();
	
public:
	static LPFN_ACCEPTEX acceptex_;		// asynchorous accept function address
	static LPFN_CONNECTEX connectex_;	// asynchorous connect function address
	static LPFN_DISCONNECTEX disconnectex_; // asynchorous disconnect function address
	static LPFN_GETACCEPTEXSOCKADDRS getacceptexsockaddrs_;	// asynchorous getsockaddr function address
};

#endif
