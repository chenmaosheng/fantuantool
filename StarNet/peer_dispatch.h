#ifndef _H_PEER_DISPATCH
#define _H_PEER_DISPATCH

#include "peer_stream.h"
#include "peer_packet.h"

#define PEER_DISPATCH_FILTER_MAX 255

struct PeerClientDispatchFilter
{
	typedef bool (CALLBACK* Func)(PEER_CLIENT, PeerInputStream&);
	Func* m_pFunc;
};

class PeerClientDispatchFilterArray
{
public:
	static PeerClientDispatchFilter& GetFilter(uint16 iFilterId);

private:
	static PeerClientDispatchFilter m_arrDispatchFilter[PEER_DISPATCH_FILTER_MAX];
};

struct PeerServerDispatchFilter
{
	typedef bool (CALLBACK* Func)(PEER_SERVER, PeerInputStream&);
	Func* m_pFunc;
};

class PeerServerDispatchFilterArray
{
public:
	static PeerServerDispatchFilter& GetFilter(uint16 iFilterId);

private:
	static PeerServerDispatchFilter m_arrDispatchFilter[PEER_DISPATCH_FILTER_MAX];
};

#endif
