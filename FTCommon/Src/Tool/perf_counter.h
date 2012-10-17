/*****************************************************************************************
	filename:	perf_counter.h
	created:	10/17/2012
	author:		chen
	purpose:	record runtime info of each function, analyze the performance

*****************************************************************************************/

#ifndef _H_PERF_COUNTER
#define _H_PERF_COUNTER

#include "common.h"
#include "singleton.h"

#define FUNC_PROFILER_MAX 32768

struct FuncProfiler
{
	FuncProfiler(const TCHAR* strFuncName);
	void Print();

	const TCHAR* m_strFuncName; // function name
	uint32 m_iCallCount; // count of function call
	uint64 m_iTimeCost; // time cost of function call, exclusively
};

struct FuncProfilerCompare
{
	bool operator()(FuncProfiler* lhs, FuncProfiler* rhs)
	{
		return lhs->m_iTimeCost > rhs->m_iTimeCost;
	}
};

struct FrameProfiler
{
	FrameProfiler(FuncProfiler* pFuncProfiler);
	~FrameProfiler();

	FuncProfiler* m_pFuncProfiler; // related function profiler
	FrameProfiler* m_pCaller; // which function call this function
	uint64 m_iTimeStart; // start time of this frame
	uint64 m_iSubTimeCostTotal; // all of sub function cost in this frame
};

class PerfCounter : public Singleton<PerfCounter>
{
public:
	PerfCounter();
	~PerfCounter();

	uint64 GetTickTime();
	uint64 GetFrequency();

	void Print();

public:
	FuncProfiler* m_arrayFuncProfiler[FUNC_PROFILER_MAX];
	uint32 m_iFuncProfilerCount;
	CRITICAL_SECTION m_csPerfCounter;

	_LARGE_INTEGER m_iFrequency;
	_LARGE_INTEGER m_iCurrTime;
};

#define PERF_PROFILER	\
	static FuncProfiler funcProfiler(_T(__FUNCTION__)); \
	FrameProfiler frameProfiler(&funcProfiler);

#endif
