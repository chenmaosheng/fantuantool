#ifndef _H_EASY_SERVER
#define _H_EASY_SERVER

#include "easy_baseserver.h"
#include "util.h"

class EasyServer : public EasyBaseServer, public Singleton<EasyServer>
{
private:
	EasyBaseLoop* _CreateServerLoop();
};

extern EasyServer* g_pServer;

#endif
