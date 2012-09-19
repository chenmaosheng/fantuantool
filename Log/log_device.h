#ifndef _H_LOG_DEVICE
#define _H_LOG_DEVICE

#include "common.h"

class LogDevice
{
public:
	virtual ~LogDevice(){}

	virtual void Init(HANDLE pHandle) = 0;
	virtual void Init(const TCHAR* strPath, const TCHAR* strFileNamePrefix, const int32 iMaxFileSize = 1<<20) = 0;

	virtual void Start() = 0;
	virtual void Stop() = 0;
	virtual bool IsRunning() const = 0;
	virtual void LogOutput(TCHAR* strBuffer) = 0;
};

#endif
