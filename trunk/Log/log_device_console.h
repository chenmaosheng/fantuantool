#ifndef _H_LOG_DEVICE_CONSOLE
#define _H_LOG_DEVICE_CONSOLE

#include "log_device_impl.h"

class LogDeviceConsole : public LogDeviceImpl
{
public:
	LogDeviceConsole(HANDLE pHandle);
	~LogDeviceConsole();

	void LogOutput(TCHAR* strBuffer);

private:
	HANDLE	m_pHandle;
};

#endif
