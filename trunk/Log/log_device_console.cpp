#include "log_device_console.h"
#include <cstdio>

LogDeviceConsole::LogDeviceConsole(HANDLE pHandle) :
	m_pHandle(pHandle)
{
	if (!m_pHandle)
	{
		m_pHandle = stdout;
	}
}

LogDeviceConsole::~LogDeviceConsole()
{
}

void LogDeviceConsole::LogOutput(TCHAR* strBuffer)
{
	fprintf_s((FILE*)m_pHandle, "%ls", strBuffer);
	fflush((FILE*)m_pHandle);
}