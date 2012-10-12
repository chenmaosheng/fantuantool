/*****************************************************************************************
	filename:	log_device_debug.h
	created:	10/12/2012
	author:		chen
	purpose:	describe a log device to output to debug window

*****************************************************************************************/

#ifndef _H_LOG_DEVICE_DEBUG
#define _H_LOG_DEVICE_DEBUG

#include "log_device_impl.h"

class LogDeviceDebug : public LogDeviceImpl
{
public:
	LogDeviceDebug();
	~LogDeviceDebug();

	void Init();
	void LogOutput(TCHAR* strBuffer);
};

#endif
