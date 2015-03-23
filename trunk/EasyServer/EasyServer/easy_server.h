#ifndef _H_EASY_SERVER
#define _H_EASY_SERVER

#include "server_base.h"
#include "util.h"

class EasyServer : public ServerBase, public Singleton<EasyServer>
{
private:
	int32 InitMainLoop();
};

extern EasyServer* g_pServer;

#endif
