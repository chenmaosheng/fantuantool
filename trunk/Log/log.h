#ifndef _H_LOG
#define _H_LOG

#include <vector>
#include "common.h"
#include "singleton.h"

class SingleBuffer;
class LogDevice;
class Log : public Singleton<Log>
{
public:
	// 每次output的大小
	enum { DELAY_MAX_SIZE = 128, BUFFER_MAX_SIZE = 65535, };

	enum
	{
		LOG_DEBUG_LEVEL,
		LOG_STATE_LEVEL,	// special level
		LOG_WARNING_LEVEL,
		LOG_ERROR_LEVEL,
	};

	Log();
	~Log();

	void Init(int32 iLogLevel, int32 iLogTypeMask);
	void Destroy();
	void Start();
	void AddLogDevice(LogDevice*);
	bool Push(int32 iLogLevel, int32 iLogType, TCHAR* strFormat, ...);

	int32 GetLogLevel() const
	{
		return m_iLogLevel;
	}

	BOOL IsInLogTypeMask(int32 iLogType)
	{
		return m_iLogTypeMask & iLogType;
	}

private:
	static uint32 WINAPI _LogOutput(PVOID);
	void _Tick();
	void _Output(TCHAR* strBuffer);

	// get string of loglevel
	TCHAR*	_Level2String(int32 iLogLevel);
	bool _Push(TCHAR* buffer, int32 iLength);

private:
	TCHAR m_strBuffer[BUFFER_MAX_SIZE];
	SingleBuffer* m_pBuffer;
	std::vector<LogDevice*> m_vLogDeviceList;
	CRITICAL_SECTION m_cs;
	HANDLE m_hOutputEvent;
	HANDLE m_hThread;

	int32 m_iLogLevel;
	int32 m_iLogTypeMask;	// need binary mask, defined by app layer
	TCHAR m_strLogType[32][128];
};

#define LOG_DBG(strFormat, Expression, ...)										\
	if (Log::Instance()->GetLogLevel() <= Log::LOG_DEBUG_LEVEL)					\
	Log::Instance()->Push(Log::LOG_DEBUG_LEVEL, 0, _T("[%-8s][%u:%-20s][LINE:%-4u] ") Expression, strFormat, GetCurrentThreadId(), _T(__FUNCTION__), __LINE__, __VA_ARGS__);

#define LOG_WAR(strFormat, Expression, ...)										\
	if (Log::Instance()->GetLogLevel() <= Log::LOG_WARNING_LEVEL)				\
	Log::Instance()->Push(Log::LOG_WARNING_LEVEL, 0, _T("[%-8s][%u:%-20s][LINE:%-4u] ") Expression, strFormat, GetCurrentThreadId(), _T(__FUNCTION__), __LINE__, __VA_ARGS__);

#define LOG_ERR(strFormat, Expression, ...)										\
	if (Log::Instance()->GetLogLevel() <= Log::LOG_ERROR_LEVEL)					\
	Log::Instance()->Push(Log::LOG_ERROR_LEVEL, 0, _T("[%-8s][%u:%-20s][LINE:%-4u] ") Expression, strFormat, GetCurrentThreadId(), _T(__FUNCTION__), __LINE__, __VA_ARGS__);

#define LOG_STT(strFormat, Expression, ...)										\
	if (Log::Instance()->GetLogLevel() <= Log::LOG_STATE_LEVEL)				\
	Log::Instance()->Push(Log::LOG_STATE_LEVEL, 0, _T("[%-8s][%u:%-20s][LINE:%-4u] ") Expression, strFormat, GetCurrentThreadId(), _T(__FUNCTION__), __LINE__, __VA_ARGS__);


#endif
