/*****************************************************************************************
	filename:	log.h
	created:	09/27/2012
	author:		chen
	purpose:	output log to different devices

*****************************************************************************************/

#ifndef _H_LOG
#define _H_LOG

#include "common.h"

class LogDevice;
class Log
{
public:
	enum
	{
		LOG_DEBUG_LEVEL,
		LOG_STATE_LEVEL,	// special level
		LOG_WARNING_LEVEL,
		LOG_ERROR_LEVEL,
	};

	enum
	{
		LOG_DEVICE_CONSOLE,
		LOG_DEVICE_FILE,
	};

	static Log* GetInstance();

	virtual void Init(int32 iLogLevel) = 0;
	virtual void Destroy() = 0;
	virtual void Start() = 0;
	virtual LogDevice* CreateAndAddLogDevice(int32 iLogDeviceType) = 0;
	virtual void SetLogTypeString(int32 iLogType, TCHAR* strLogTypeString) = 0;
	virtual bool Push(int32 iLogLevel, TCHAR* strFormat, ...) = 0;

	virtual int32 GetLogLevel() const = 0;
	virtual const TCHAR* GetLogTypeString(int32 iLogType) const = 0;
};

#define LOG_DBG(LogType, Expression, ...)										\
	if (Log::GetInstance()->GetLogLevel() <= Log::LOG_DEBUG_LEVEL)					\
	Log::GetInstance()->Push(Log::LOG_DEBUG_LEVEL, _T("[%-8s][%u:%-30s][LINE:%-4u] ") Expression, Log::GetInstance()->GetLogTypeString(LogType), GetCurrentThreadId(), _T(__FUNCTION__), __LINE__, __VA_ARGS__);

#define LOG_WAR(LogType, Expression, ...)										\
	if (Log::GetInstance()->GetLogLevel() <= Log::LOG_WARNING_LEVEL)				\
	Log::GetInstance()->Push(Log::LOG_WARNING_LEVEL, _T("[%-8s][%u:%-30s][LINE:%-4u] ") Expression, Log::GetInstance()->GetLogTypeString(LogType), GetCurrentThreadId(), _T(__FUNCTION__), __LINE__, __VA_ARGS__);

#define LOG_ERR(LogType, Expression, ...)										\
	if (Log::GetInstance()->GetLogLevel() <= Log::LOG_ERROR_LEVEL)					\
	Log::GetInstance()->Push(Log::LOG_ERROR_LEVEL, _T("[%-8s][%u:%-30s][LINE:%-4u] ") Expression, Log::GetInstance()->GetLogTypeString(LogType), GetCurrentThreadId(), _T(__FUNCTION__), __LINE__, __VA_ARGS__);

#define LOG_STT(LogType, Expression, ...)										\
	if (Log::GetInstance()->GetLogLevel() <= Log::LOG_STATE_LEVEL)				\
	Log::GetInstance()->Push(Log::LOG_STATE_LEVEL, _T("[%-8s][%u:%-30s][LINE:%-4u] ") Expression, Log::GetInstance()->GetLogTypeString(LogType), GetCurrentThreadId(), _T(__FUNCTION__), __LINE__, __VA_ARGS__);


#endif
