/*****************************************************************************************
	filename:	starnet.h
	created:	09/27/2012
	author:		chen
	purpose:	starnet starter

*****************************************************************************************/

#ifndef _H_STAR_NET
#define _H_STAR_NET

#include "starnet_common.h"

class StarNet
{
public:
	// intialize starnet
	static int32 Init();
	// destroy starnet
	static void Destroy();
	// create and start a server for other peer servers
	static bool StartPeerServer(uint32 iIP, uint16 iPort, uint32 iWorkerCount);
	// stop a peer server
	static void StopPeerServer();
	// get a remote peer server by ip and port
	static PEER_SERVER GetPeerServer(uint32 iIP, uint16 iPort);

public:
	static LPFN_ACCEPTEX acceptex_;		// asynchorous accept function address
	static LPFN_CONNECTEX connectex_;	// asynchorous connect function address
	static LPFN_DISCONNECTEX disconnectex_; // asynchorous disconnect function address
	static LPFN_GETACCEPTEXSOCKADDRS getacceptexsockaddrs_;	// asynchorous getsockaddr function address
};


#endif
