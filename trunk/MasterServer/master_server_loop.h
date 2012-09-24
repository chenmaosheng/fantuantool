#ifndef _H_MASTER_SERVER_LOOP
#define _H_MASTER_SERVER_LOOP

#include "logic_loop.h"
#include "object_pool.h"
#include <hash_map>

class MasterPlayerContext;
class MasterServerLoop : public LogicLoop
{
public:
	typedef LogicLoop super;

	MasterServerLoop();
	
	int32 Init();
	void Destroy();

	int32 Start();
	bool IsReadyForShutdown() const;

private:
	uint32 _Loop();
	void _ShutdownPlayer(MasterPlayerContext*);
	
	bool _OnCommand(LogicCommand*);
	void _OnCommandShutdown();	

private:
	int32 m_iShutdownStatus;
	
	ObjectPool<MasterPlayerContext> m_PlayerContextPool;
	stdext::hash_map<const TCHAR*, MasterPlayerContext*> m_mPlayerContextByName;
	stdext::hash_map<uint32, MasterPlayerContext*> m_mPlayerContextBySessionId;
};

#endif
