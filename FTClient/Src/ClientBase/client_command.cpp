#include "client_command.h"

ClientCommandOnData::ClientCommandOnData()
{
	m_iLen = 0;
	m_pData = NULL;
}

ClientCommandOnData::~ClientCommandOnData()
{
	_aligned_free(m_pData);
}

bool ClientCommandOnData::CopyData(uint16 iLen, const char *pData)
{
	m_pData = (char*)_aligned_malloc(iLen, MEMORY_ALLOCATION_ALIGNMENT);
	if (m_pData)
	{
		m_iLen = iLen;
		memcpy(m_pData, pData, iLen);
		return true;
	}

	return false;
}