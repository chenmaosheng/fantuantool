#ifndef _H_EASY_SERVER_LOOP
#define _H_EASY_SERVER_LOOP

#include "easy_session.h"
#include "logic_loop.h"

struct LogicCommandOnPingReq;
class EasyServerLoop : public LogicLoop
{
public:
	EasyServerLoop();
	~EasyServerLoop();

	int32 Init();
	
private:
	Session* _CreateSession();
	bool _OnCommand(LogicCommand*);
	bool _OnCommandOnPingReq(LogicCommandOnPingReq*);
};

#endif
