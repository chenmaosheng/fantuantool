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

void MasterPlayerContext::OnLoginReq(uint32 iSessionId, const TCHAR* strAccountName)
{
	int32 iRet = 0;

	m_iSessionId = iSessionId;
	wcscpy_s(m_strAccountName, sizeof(m_strAccountName)/sizeof(TCHAR), strAccountName);

	// notify frontend
}
