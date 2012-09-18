#ifndef _H_LOG_DEVICE_FILE
#define _H_LOG_DEVICE_FILE

#include "log_device_impl.h"

class LogDeviceFile : public LogDeviceImpl
{
public:
	enum { FILENAME_MAXSIZE = 1<<8, };

	LogDeviceFile(const TCHAR* strPath, const TCHAR* strFileNamePrefix, const int32 iMaxFileSize = 1<<20);
	~LogDeviceFile();

	void LogOutput(TCHAR* strBuffer);

private:
	void _FileOpen(const TCHAR* strPath, const TCHAR* strFileName);
	void _FileClose();

private:
	TCHAR m_strFileName[FILENAME_MAXSIZE];
	FILE* m_pFile;
};

#endif
