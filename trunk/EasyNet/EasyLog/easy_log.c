#include "easy_log.h"
#include <process.h>
#include "easy_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

EasyLog easy_log_;

uint32 WINAPI OutputEasyLog(PVOID pParam)
{
	uint16 iCurrSize = 0;
	uint16 iLength = 0;
	DWORD dwBytesWritten = 0;
	
	while (TRUE)
	{
		DWORD ret = WaitForSingleObject(easy_log_.event_, INFINITE);
		if (ret == WAIT_OBJECT_0)
		{
			EnterCriticalSection(&easy_log_.cs_);
			iCurrSize = (uint16)easy_log_.easy_buffer_->curr_size_;
			memset(easy_log_.buffer_, 0, sizeof(easy_log_.buffer_));
			iLength = PopFromEasyBuffer(easy_log_.easy_buffer_, easy_log_.buffer_, iCurrSize);
			if (iLength)
			{
				ResetEvent(easy_log_.event_);
				wprintf_s(_T("%s"), easy_log_.buffer_);
				OutputDebugString(easy_log_.buffer_);
				
				WriteFile(easy_log_.file_, easy_log_.buffer_, iLength, &dwBytesWritten, NULL);
			}
			LeaveCriticalSection(&easy_log_.cs_);
		}
	}

	return 0;
}

TCHAR* Level2String(int32 iLogLevel)
{
	switch(iLogLevel)
	{
	case LOG_DEBUG_LEVEL:
		return TEXT("DEBUG");

	case LOG_WARNING_LEVEL:
		return TEXT("WARNI");

	case LOG_ERROR_LEVEL:
		return TEXT("ERROR");

	case LOG_STATE_LEVEL:
		return TEXT("STATE");

	default:
		return TEXT("");
	}
}

void StartEasyLog(int32 iLogLevel)
{
	DWORD dwBytesWritten = 0;
	DWORD a=0xFEFF;

	InitializeCriticalSectionAndSpinCount(&easy_log_.cs_, 4000);
	easy_log_.easy_buffer_ = CreateEasyBuffer(BUFFER_MAX_SIZE);
	memset(easy_log_.buffer_, 0, sizeof(easy_log_.buffer_)); 
	easy_log_.level_ = iLogLevel;
	easy_log_.file_ = CreateFile(_T("e://test.log"), GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
	WriteFile(easy_log_.file_,&a,sizeof(a),&dwBytesWritten,NULL);

	easy_log_.event_ = CreateEvent(NULL, TRUE, FALSE, NULL);
	easy_log_.thread_ = (HANDLE)_beginthreadex(NULL, 0, &OutputEasyLog, NULL, 0, NULL);
}

void StopEasyLog()
{
	DeleteCriticalSection(&easy_log_.cs_);
	_aligned_free(easy_log_.easy_buffer_);
	CloseHandle(easy_log_.file_);
	CloseHandle(easy_log_.event_);
	CloseHandle(easy_log_.thread_);
}

int32 GetEasyLogLevel()
{
	return easy_log_.level_;
}

BOOL PushEasyLog(int32 iLogLevel, TCHAR* strFormat, ...)
{
	TCHAR buffer[BUFFER_MAX_SIZE];
	TCHAR realBuffer[BUFFER_MAX_SIZE];
	int32 iLength = 0;
	SYSTEMTIME now;
	BOOL bRet = FALSE;

	va_list Args;
	va_start(Args, strFormat);
	iLength = _vsctprintf(strFormat, Args) + 1;
	if (iLength <= BUFFER_MAX_SIZE)
	{
		_vstprintf_s(buffer, iLength, strFormat, Args);
		GetLocalTime(&now);
		_stprintf_s(realBuffer, BUFFER_MAX_SIZE, _T("[%02d.%02d.%02d %02d:%02d:%02d][%s]%s\r\n"), 
			now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond,
			Level2String(iLogLevel), buffer);
		
		EnterCriticalSection(&easy_log_.cs_);
		SetEvent(easy_log_.event_);
		bRet = PushToEasyBuffer(easy_log_.easy_buffer_, realBuffer, (int32)wcslen(realBuffer)*sizeof(TCHAR));
		LeaveCriticalSection(&easy_log_.cs_);

		return bRet;
	}
	
	return FALSE;
}

#ifdef __cplusplus
}
#endif
