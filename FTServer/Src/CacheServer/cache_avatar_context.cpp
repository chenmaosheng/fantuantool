#include "cache_avatar_context.h"

CacheAvatarContext::CacheAvatarContext()
{
	Clear();
}

CacheAvatarContext::~CacheAvatarContext()
{
	Clear();
}

void CacheAvatarContext::Clear()
{
	m_strAvatarName[0] = _T('\0');
	m_iAvatarId = 0;

	m_iLevel = 0;

	memset(m_arrayDirty, 0, sizeof(m_arrayDirty));
}