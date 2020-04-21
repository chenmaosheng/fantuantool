/*****************************************************************************************
	filename:	peer_dispatch.h
	created:	09/27/2012
	author:		chen
	purpose:	packet dispatcher, only for Peer

*****************************************************************************************/

#ifndef _H_PEER_DISPATCH
#define _H_PEER_DISPATCH

#include "peer_stream.h"

#define PEER_DISPATCH_FILTER_MAX 255

// one filter of dispatcher, contains several functions
struct PeerClientDispatchFilter
{
	typedef bool (CALLBACK* Func)(PEER_CLIENT, PeerInputStream&);
	uint32 m_iFuncCount;
	Func* m_pFunc;
};

// lot of filters of dispatcher
class PeerClientDispatchFilterArray
{
public:
	static PeerClientDispatchFilter& GetFilter(uint16 iFilterId);

private:
	static PeerClientDispatchFilter m_arrDispatchFilter[PEER_DISPATCH_FILTER_MAX];
};

// one filter of dispatcher, contains several functions
struct PeerServerDispatchFilter
{
	typedef bool (CALLBACK* Func)(PEER_SERVER, PeerInputStream&);
	uint32 m_iFuncCount;
	Func* m_pFunc;
};

// lot of filters of dispatcher
class PeerServerDispatchFilterArray
{
public:
	static PeerServerDispatchFilter& GetFilter(uint16 iFilterId);

private:
	static PeerServerDispatchFilter m_arrDispatchFilter[PEER_DISPATCH_FILTER_MAX];
};

#endif
