#include "logic_command.h"

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

LogicCommandBroadcastData::LogicCommandBroadcastData()
{
	m_iSessionCount = 0;
	m_iLen = 0;
	m_iTypeId = 0;
	m_pData = NULL;
	memset(m_arraySessionId, 0, sizeof(m_arraySessionId));
}

LogicCommandBroadcastData::~LogicCommandBroadcastData()
{
	_aligned_free(m_pData);
}

bool LogicCommandBroadcastData::CopyData(uint16 iLen, const char *pData)
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

LogicCommandPacketForward::LogicCommandPacketForward()
{
	m_iSessionId = 0;
	m_iTypeId = 0;
	m_iLen = 0;
	m_pData = NULL;
}

LogicCommandPacketForward::~LogicCommandPacketForward()
{
	_aligned_free(m_pData);
}

bool LogicCommandPacketForward::CopyData(uint16 iLen, const char *pData)
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