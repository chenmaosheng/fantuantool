/*****************************************************************************************
	filename:	master_server_loop.h
	created:	09/27/2012
	author:		chen
	purpose:	control game tick on master server

*****************************************************************************************/

#ifndef _H_MASTER_SERVER_LOOP
#define _H_MASTER_SERVER_LOOP

#include "logic_loop.h"
#include "object_pool.h"
#include <hash_map>
#include <string>

class MasterPlayerContext;
struct LogicCommandOnLoginReq;
struct LogicCommandGateHoldAck;
class MasterServerLoop : public LogicLoop
{
public:
	typedef LogicLoop super;

	// cstr
	MasterServerLoop();
	
	// initialize master server loop
	int32 Init();
	// destroy master server loop
	void Destroy();
	// start master server loop
	int32 Start();
	// check if is ready for shutdown
	bool IsReadyForShutdown() const;

	// receive request about hold a gate session
	int32 GateHoldReq();

private:
	uint32 _Loop();
	// shutdown one player in master server
	void _ShutdownPlayer(MasterPlayerContext*);
	
	bool _OnCommand(LogicCommand*);
	void _OnCommandShutdown();

private:
	// receive request about login
	void _OnCommandOnLoginReq(LogicCommandOnLoginReq*);
	// acknowledge response about hold a gate session
	void _OnCommandGateHoldAck(LogicCommandGateHoldAck*);

private:
	int32 m_iShutdownStatus;
	
	ObjectPool<MasterPlayerContext> m_PlayerContextPool;
	stdext::hash_map<std::wstring, MasterPlayerContext*> m_mPlayerContextByName;
	stdext::hash_map<uint32, MasterPlayerContext*> m_mPlayerContextBySessionId;
};

#endif
