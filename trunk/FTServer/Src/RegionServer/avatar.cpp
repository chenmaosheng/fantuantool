#include "avatar.h"

Avatar::Avatar() : m_StateMachine(0)
{
	m_iActorType = ACTOR_TYPE_AVATAR;
	m_pPlayerContext = NULL;
	m_bFinalizing = false;
	
	m_strAvatarName[0] = _T('\0');
	m_iAvatarId = 0;

	_InitStateMachine();
}

Avatar::~Avatar()
{
}



