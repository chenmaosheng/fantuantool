#ifndef _H_LOGIC_LOOP
#define _H_LOGIC_LOOP

#include "common.h"

class LogicLoop
{
public:
	LogicLoop();
	virtual ~LogicLoop();

	virtual int32 Init(bool bMainLoop = true);
	virtual void Destroy();

	virtual int32 Start();
	virtual void Stop();
	virtual bool IsAllowStop() = 0;

	virtual void Join();
	virtual void Pause();
	virtual void Resume();

	virtual void PushCommand(
};

#endif
