#include "log_device_debug.h"

LogDeviceDebug::LogDeviceDebug()
{
}

LogDeviceDebug::~LogDeviceDebug()
{
}

void LogDeviceDebug::Init()
{
}

void LogDeviceDebug::LogOutput(TCHAR* strBuffer, uint16 iCount)
{
	OutputDebugString(strBuffer);
}