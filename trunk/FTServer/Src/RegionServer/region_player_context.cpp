#include "region_player_context.h"
#include "region_server_loop.h"

RegionServerLoop* RegionPlayerContext::m_pMainLoop = NULL;

RegionPlayerContext::RegionPlayerContext() :
m_StateMachine(PLAYER_STATE_NONE)
{
	Clear();
}

RegionPlayerContext::~RegionPlayerContext()
{
	Clear();
}

void RegionPlayerContext::Clear()
{
	m_iSessionId = 0;
	m_iAvatarId = 0;
	m_strAvatarName[0] = _T('\0');

	m_bFinalizing = false;
	m_pGateServer = NULL;
	m_StateMachine.SetCurrState(PLAYER_STATE_NONE);

	m_iMapId = 0;
}

void RegionPlayerContext::OnRegionAllocReq(uint32 iSessionId, uint64 iAvatarId, const TCHAR* strAvatarName)
{
	// todo:
}
