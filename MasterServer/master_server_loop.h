#ifndef _H_MASTER_SERVER_LOOP
#define _H_MASTER_SERVER_LOOP

#include "logic_loop.h"

class MasterServerLoop : public LogicLoop
{
public:
	typedef LogicLoop super;

	MasterServerLoop();
	~MasterServerLoop();

	int32 Init();
	void Destroy();

	int32 Start();
	bool IsReadyForShutdown() const;

private:
	uint32 _Loop();
	bool _OnCommand(LogicCommand*);
};

#endif
