#include "log.h"
#include "single_buffer.h"
#include "log_device.h"
#include <process.h>

Log::Log() : 
	m_hThread(NULL)
{
	memset(m_strBuffer, 0, sizeof(m_strBuffer));
	m_pBuffer = new SingleBuffer(m_iMaxSize);
	::InitializeCriticalSection(&m_cs);
	// manual notification
	m_hOutputEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
}

Log::~Log()
{
	::DeleteCriticalSection(&m_cs);
	SAFE_DELETE(m_pBuffer);
	CloseHandle(m_hOutputEvent);
	CloseHandle(m_hThread);
}

void Log::AddLogDevice(LogDevice* pDevice)
{
	m_vLogDeviceList.push_back(pDevice);
}

bool Log::Push(TCHAR *strBuffer, uint16 iLength)
{
	bool bRet = false;
	::EnterCriticalSection(&m_cs);
	::SetEvent(m_hOutputEvent);
	bRet = m_pBuffer->Push(strBuffer, iLength * sizeof(TCHAR));
	::LeaveCriticalSection(&m_cs);
	return bRet;
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