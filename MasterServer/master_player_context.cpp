#include "master_player_context.h"
#include "master_server_loop.h"

MasterServerLoop* MasterPlayerContext::m_pMainLoop = NULL;

MasterPlayerContext::MasterPlayerContext()
{
	Clear();
}

MasterPlayerContext::~MasterPlayerContext()
{
	Clear();
}

void MasterPlayerContext::Clear()
{
	m_iSessionId = 0;
	m_strAccountName[0] = '\0';
}

