/*****************************************************************************************
	filename:	log_device_console.h
	created:	09/27/2012
	author:		chen
	purpose:	describe a log device to output to console

*****************************************************************************************/

#ifndef _H_LOG_DEVICE_CONSOLE
#define _H_LOG_DEVICE_CONSOLE

#include "log_device_impl.h"

class LogDeviceConsole : public LogDeviceImpl
{
public:
	LogDeviceConsole(HANDLE pHandle = NULL);
	~LogDeviceConsole();

	void Init(HANDLE pHandle);
	void LogOutput(TCHAR* strBuffer, uint16 iCount);

private:
	void _SetColor(TCHAR* strBuffer);

private:
	HANDLE	m_pHandle;
};

#endif
