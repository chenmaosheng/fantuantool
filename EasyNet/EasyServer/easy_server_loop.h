#ifndef _H_EASY_SERVER_LOOP
#define _H_EASY_SERVER_LOOP

#include "easy_session.h"
#include "easy_baseloop.h"

struct LogicCommandOnPingReq;
class EasyServerLoop : public EasyBaseLoop
{
public:
	EasyServerLoop();
	~EasyServerLoop();

	int32 Init();
	
private:
	EasyBaseSession* _CreateSession();
};

#endif
