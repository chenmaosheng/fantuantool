#include "log.h"
#include "single_buffer.h"
#include "log_device.h"
#include <process.h>
#include <cstdio>

Log::Log() : 
	m_hThread(NULL),
	m_iLogLevel(LOG_DEBUG_LEVEL),
	m_iLogTypeMask(0)
{
	memset(m_strBuffer, 0, sizeof(m_strBuffer));
	m_pBuffer = new SingleBuffer(BUFFER_MAX_SIZE);
	::InitializeCriticalSection(&m_cs);
	// manual notification
	m_hOutputEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	memset(m_strLogType, 0, sizeof(m_strLogType));
}

Log::~Log()
{
	::DeleteCriticalSection(&m_cs);
	SAFE_DELETE(m_pBuffer);
	CloseHandle(m_hOutputEvent);
	CloseHandle(m_hThread);
}

void Log::Init(int32 iLogLevel, int32 iLogTypeMask)
{
	m_iLogLevel = 0;
	m_iLogTypeMask = 0;
}

void Log::Destroy()
{
	for (std::vector<LogDevice*>::iterator it = m_vLogDeviceList.begin(); it != m_vLogDeviceList.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_vLogDeviceList.clear();
}

void Log::AddLogDevice(LogDevice* pDevice)
{
	m_vLogDeviceList.push_back(pDevice);
}

bool Log::Push(int32 iLogLevel, int32 iLogType, TCHAR* strFormat, ...)
{
	TCHAR Buffer[BUFFER_MAX_SIZE] = {0};
	TCHAR RealBuffer[BUFFER_MAX_SIZE] = {0};

	va_list Args;
	va_start(Args, strFormat);
	int32 iLength = ::_vsctprintf(strFormat, Args) + 1;
	if (iLength <= BUFFER_MAX_SIZE)
	{
		::_vstprintf_s(Buffer, iLength, strFormat, Args);
		SYSTEMTIME now;
		GetLocalTime(&now);
		_stprintf_s(RealBuffer, _T("[%02d.%02d.%02d %02d:%02d:%02d][%s]%s\n"), 
			now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond,
			_Level2String(iLogLevel), Buffer);
		return _Push(RealBuffer, (int32)wcslen(RealBuffer));
	}

	return false;
}

void Log::Start()
{
	for (std::vector<LogDevice*>::iterator it = m_vLogDeviceList.begin(); it != m_vLogDeviceList.end(); ++it)
	{
		(*it)->Start();
	}

	m_hThread = (HANDLE)::_beginthreadex(NULL, 0, &Log::_LogOutput, this, 0, NULL);
}

uint32 WINAPI Log::_LogOutput(PVOID pParam)
{
	Log* pLog = (Log*)pParam;
	while (true)
	{
		pLog->_Tick();
	}

	return 0;
}

void Log::_Tick()
{
	if (m_hOutputEvent)
	{
		DWORD ret = ::WaitForSingleObject(m_hOutputEvent, INFINITE);
		if (ret == WAIT_OBJECT_0)
		{
			::EnterCriticalSection(&m_cs);
			uint16 iCurrSize = (uint16)m_pBuffer->GetCurrSize();
			memset(m_strBuffer, 0, sizeof(m_strBuffer));
			if (m_pBuffer->Pop(m_strBuffer, iCurrSize))
			{
				ResetEvent(m_hOutputEvent);
				_Output(m_strBuffer);
			}
			::LeaveCriticalSection(&m_cs);
		}
	}
}

void Log::_Output(TCHAR* strBuffer)
{
	std::vector<LogDevice*>::iterator it = m_vLogDeviceList.begin();
	while (it != m_vLogDeviceList.end())
	{
		if ((*it)->IsRunning())
		{
			(*it)->LogOutput(strBuffer);
		}

		++it;
	}
}

TCHAR* Log::_Level2String(int32 iLogLevel)
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

bool Log::_Push(TCHAR *buffer, int32 iLength)
{
	bool bRet = false;
	::EnterCriticalSection(&m_cs);
	::SetEvent(m_hOutputEvent);
	bRet = m_pBuffer->Push(buffer, iLength * sizeof(TCHAR));
	::LeaveCriticalSection(&m_cs);
	return bRet;
}