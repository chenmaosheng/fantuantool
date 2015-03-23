#ifndef _H_EASY_LOG
#define _H_EASY_LOG

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BUFFER_MAX_SIZE 65535

enum
{
	LOG_DEBUG_LEVEL,
	LOG_STATE_LEVEL,	// special level
	LOG_WARNING_LEVEL,
	LOG_ERROR_LEVEL,
};

struct _EasyBuffer;
typedef struct _EasyLog
{
	int32 level_;
	HANDLE event_;
	HANDLE thread_;
	HANDLE file_;
	CRITICAL_SECTION cs_;
	struct _EasyBuffer* easy_buffer_;
	TCHAR buffer_[BUFFER_MAX_SIZE];
}EasyLog;

void StartEasyLog(int32 iLogLevel);
void StopEasyLog();
BOOL PushEasyLog(int32 iLogLevel, TCHAR* strFormat, ...);
int32 GetEasyLogLevel();

#define LOG_DBG(Expression, ...)										\
	if (GetEasyLogLevel() <= LOG_DEBUG_LEVEL)					\
	PushEasyLog(LOG_DEBUG_LEVEL, _T("[%u:%-30s][LINE:%-4u] ") Expression, GetCurrentThreadId(), _T(__FUNCTION__), __LINE__, __VA_ARGS__);

#define LOG_WAR(Expression, ...)										\
	if (GetEasyLogLevel() <= LOG_WARNING_LEVEL)				\
	PushEasyLog(LOG_WARNING_LEVEL, _T("[%u:%-30s][LINE:%-4u] ") Expression, GetCurrentThreadId(), _T(__FUNCTION__), __LINE__, __VA_ARGS__);

#define LOG_ERR(Expression, ...)										\
	if (GetEasyLogLevel() <= LOG_ERROR_LEVEL)					\
	PushEasyLog(LOG_ERROR_LEVEL, _T("[%u:%-30s][LINE:%-4u] ") Expression, GetCurrentThreadId(), _T(__FUNCTION__), __LINE__, __VA_ARGS__);

#define LOG_STT(Expression, ...)										\
	if (GetEasyLogLevel() <= LOG_STATE_LEVEL)				\
	PushEasyLog(LOG_STATE_LEVEL, _T("[%u:%-30s][LINE:%-4u] ") Expression, GetCurrentThreadId(), _T(__FUNCTION__), __LINE__, __VA_ARGS__);



#ifdef __cplusplus
}
#endif

#endif
