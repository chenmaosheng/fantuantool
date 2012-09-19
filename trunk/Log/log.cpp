#include "log.h"
#include "log_device.h"
#include "log_device_console.h"
#include "log_device_file.h"

#include <process.h>
#include <cstdio>
#include <vector>

#include "singleton.h"
#include "single_buffer.h"

class Log_Impl : public Log, 
				public Singleton<Log_Impl>
{
public:
	enum { DELAY_MAX_SIZE = 128, BUFFER_MAX_SIZE = 65535, LOGTYPE_MAX_SIZE = LOG_TYPE_MAX, LOGTYPE_NAME_MAX = MAX_PATH};

	Log_Impl();
	~Log_Impl();

	void Init(int32 iLogLevel);
	void Destroy();
	void Start();
	LogDevice* CreateAndAddLogDevice(int32 iLogDeviceType);
	void SetLogTypeString(int32 iLogType, TCHAR* strLogTypeString);
	bool Push(int32 iLogLevel, TCHAR* strFormat, ...);

	int32 GetLogLevel() const
	{
		return m_iLogLevel;
	}

	const TCHAR* GetLogTypeString(int32 iLogType) const
	{
		if (iLogType < LOGTYPE_MAX_SIZE)
		{
			return m_strLogType[iLogType];
		}

		return _T("");
	}

private:
	static uint32 WINAPI _LogOutput(PVOID);
	void _Tick();
	void _Output(TCHAR* strBuffer);
	LogDevice* _AddLogDevice(LogDevice*);

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
	TCHAR m_strLogType[LOGTYPE_MAX_SIZE][LOGTYPE_NAME_MAX];
};

Log* Log::GetInstance()
{
	return Log_Impl::Instance();
}

Log_Impl::Log_Impl() : 
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

Log_Impl::~Log_Impl()
{
	::DeleteCriticalSection(&m_cs);
	SAFE_DELETE(m_pBuffer);
	CloseHandle(m_hOutputEvent);
	CloseHandle(m_hThread);
}

void Log_Impl::Init(int32 iLogLevel)
{
	m_iLogLevel = iLogLevel;
}

void Log_Impl::SetLogTypeString(int32 iLogType, TCHAR* strLogTypeString)
{
	if (iLogType)
	{
		wcscpy_s(m_strLogType[iLogType], strLogTypeString);
	}
}

void Log_Impl::Destroy()
{
	for (std::vector<LogDevice*>::iterator it = m_vLogDeviceList.begin(); it != m_vLogDeviceList.end(); ++it)
	{
		SAFE_DELETE(*it);
	}

	m_vLogDeviceList.clear();
}

LogDevice* Log_Impl::CreateAndAddLogDevice(int32 iLogDeviceType)
{
	LogDevice* pDevice = NULL;
	switch(iLogDeviceType)
	{
	case LOG_DEVICE_CONSOLE:
		return _AddLogDevice(new LogDeviceConsole);

	case LOG_DEVICE_FILE:
		return _AddLogDevice(new LogDeviceFile);
	}

	return NULL;
}

bool Log_Impl::Push(int32 iLogLevel, TCHAR* strFormat, ...)
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

void Log_Impl::Start()
{
	for (std::vector<LogDevice*>::iterator it = m_vLogDeviceList.begin(); it != m_vLogDeviceList.end(); ++it)
	{
		(*it)->Start();
	}

	m_hThread = (HANDLE)::_beginthreadex(NULL, 0, &Log_Impl::_LogOutput, this, 0, NULL);
}

LogDevice* Log_Impl::_AddLogDevice(LogDevice* pDevice)
{
	m_vLogDeviceList.push_back(pDevice);
	return pDevice;
}

uint32 WINAPI Log_Impl::_LogOutput(PVOID pParam)
{
	Log_Impl* pLog = (Log_Impl*)pParam;
	while (true)
	{
		pLog->_Tick();
	}

	return 0;
}

void Log_Impl::_Tick()
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

void Log_Impl::_Output(TCHAR* strBuffer)
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

TCHAR* Log_Impl::_Level2String(int32 iLogLevel)
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

bool Log_Impl::_Push(TCHAR *buffer, int32 iLength)
{
	bool bRet = false;
	::EnterCriticalSection(&m_cs);
	::SetEvent(m_hOutputEvent);
	bRet = m_pBuffer->Push(buffer, iLength * sizeof(TCHAR));
	::LeaveCriticalSection(&m_cs);
	return bRet;
}