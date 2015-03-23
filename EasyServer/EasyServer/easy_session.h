#ifndef _H_EASY_SESSION
#define _H_EASY_SESSION

#include "session.h"

class EasyServerLoop;
class EasySession : public Session
{
public:
	void OnPingReq(uint32 iVersion);

public:
	static EasyServerLoop* m_pMainLoop;
};

#endif
