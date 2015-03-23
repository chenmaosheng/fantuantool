#include "easy_basecommand.h"

LogicCommandOnData::LogicCommandOnData()
{
	m_iLen = 0;
	m_pData = NULL;
}

LogicCommandOnData::~LogicCommandOnData()
{
	_aligned_free(m_pData);
}

bool LogicCommandOnData::CopyData(uint16 iLen, const char *pData)
{
	m_pData = (char*)_aligned_malloc(iLen, MEMORY_ALLOCATION_ALIGNMENT);
	_ASSERT(m_pData);
	if (m_pData)
	{
		m_iLen = iLen;
		memcpy(m_pData, pData, iLen);
		return true;
	}

	return false;
}

LogicCommandSendData::LogicCommandSendData()
{
	m_iSessionId = 0;
	m_iTypeId = 0;
	m_iLen = 0;
	m_pData = NULL;
}

LogicCommandSendData::~LogicCommandSendData()
{
	_aligned_free(m_pData);
}

bool LogicCommandSendData::CopyData(uint16 iLen, const char* pData)
{
	m_pData = (char*)_aligned_malloc(iLen, MEMORY_ALLOCATION_ALIGNMENT);
	_ASSERT(m_pData);
	if (m_pData)
	{
		m_iLen = iLen;
		memcpy(m_pData, pData, iLen);
		return true;
	}

	return false;
}
