#include "perf_counter.h"
#include "auto_locker.h"
#include <list>

static __declspec(thread) FrameProfiler* g_pLastFrameProfiler = NULL;

PerfCounter::PerfCounter()
{
	InitializeCriticalSectionAndSpinCount(&m_csPerfCounter, 4000);
	memset(m_arrayFuncProfiler, 0, sizeof(m_arrayFuncProfiler));
	m_iFuncProfilerCount = 0;

	QueryPerformanceFrequency(&m_iFrequency);
}

PerfCounter::~PerfCounter()
{
	DeleteCriticalSection(&m_csPerfCounter);
}

uint64 PerfCounter::GetTickTime()
{
	QueryPerformanceCounter(&m_iCurrTime);
	return m_iCurrTime.QuadPart;
}

uint64 PerfCounter::GetFrequency()
{
	return m_iFrequency.QuadPart;
}

FuncProfiler::FuncProfiler(const TCHAR* strFuncName) : m_strFuncName(strFuncName)
{
	m_iCallCount = 0;
	m_iTimeCost = 0;
	
	AutoLocker locker(&PerfCounter::Instance()->m_csPerfCounter);
	PerfCounter::Instance()->m_arrayFuncProfiler[PerfCounter::Instance()->m_iFuncProfilerCount++] = this;
}

FrameProfiler::FrameProfiler(FuncProfiler* pFuncProfiler) : m_pFuncProfiler(pFuncProfiler)
{
	m_iTimeStart = PerfCounter::Instance()->GetTickTime();
	m_pCaller = g_pLastFrameProfiler;
	g_pLastFrameProfiler = this;
	m_iSubTimeCostTotal = 0;
}

FrameProfiler::~FrameProfiler()
{
	uint64 iTimeCost = PerfCounter::Instance()->GetTickTime() - m_iTimeStart;
	g_pLastFrameProfiler = m_pCaller;

	if (m_pFuncProfiler)
	{
		AutoLocker locker(&PerfCounter::Instance()->m_csPerfCounter);
		m_pFuncProfiler->m_iCallCount++;
		m_pFuncProfiler->m_iTimeCost += (iTimeCost - m_iSubTimeCostTotal);
	}

	if (m_pCaller)
	{
		m_pCaller->m_iSubTimeCostTotal += (PerfCounter::Instance()->GetTickTime() - m_iTimeStart);
	}
}

void PerfCounter::Print()
{
	AutoLocker locker(&m_csPerfCounter);

	std::list<FuncProfiler*> funcProfileList;
	for (uint32 i = 0; i < m_iFuncProfilerCount; ++i)
	{
		if (m_arrayFuncProfiler[i])
		{
			funcProfileList.push_back(m_arrayFuncProfiler[i]);
		}
	}

	funcProfileList.sort(FuncProfilerCompare());

	for (std::list<FuncProfiler*>::iterator it = funcProfileList.begin(); it != funcProfileList.end(); ++it)
	{
		(*it)->Print();
	}
}