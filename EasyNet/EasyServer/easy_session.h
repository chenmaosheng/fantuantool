#ifndef _H_EASY_SESSION
#define _H_EASY_SESSION

#include "easy_basesession.h"

class EasyServerLoop;
class EasySession : public EasyBaseSession
{
public:
	void OnPingReq(uint32 iVersion);

public:
	static EasyServerLoop* m_pMainLoop;
};

#endif
