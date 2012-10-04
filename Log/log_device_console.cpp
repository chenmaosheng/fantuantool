#include "log_device_console.h"
#include <cstdio>

LogDeviceConsole::LogDeviceConsole(HANDLE pHandle) :
	m_pHandle(pHandle)
{
	if (!m_pHandle)
	{
		m_pHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	}
}

LogDeviceConsole::~LogDeviceConsole()
{
}

void LogDeviceConsole::Init(HANDLE pHandle)
{
	m_pHandle = pHandle;
}

void LogDeviceConsole::LogOutput(TCHAR* strBuffer)
{
	_SetColor(strBuffer);
	wprintf_s(_T("%s"), strBuffer);
}

void LogDeviceConsole::_SetColor(TCHAR* strBuffer)
{
	int32 iLogLevel = 0;
	if (_tcsstr(strBuffer, _T("DEBUG")))
	{
		iLogLevel = 0;
	}
	else
	if (_tcsstr(strBuffer, _T("WARNI")))
	{
		iLogLevel = 2;
	}
	else
	if (_tcsstr(strBuffer, _T("ERROR")))
	{
		iLogLevel = 3;
	}
	else
	{
		iLogLevel = 1;
	}

	WORD	color = 0;
	if (iLogLevel <= 1)
	{
		color = 0x0007;
	}
	else
	if (iLogLevel <= 2)
	{
		color = FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY;
	}
	else
	{
		color = FOREGROUND_RED | FOREGROUND_INTENSITY;
	}

	SetConsoleTextAttribute( m_pHandle, color );
}