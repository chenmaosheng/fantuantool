/*****************************************************************************************
	filename:	log_device_file.h
	created:	09/27/2012
	author:		chen
	purpose:	describe a log device to output to file

*****************************************************************************************/

#ifndef _H_LOG_DEVICE_FILE
#define _H_LOG_DEVICE_FILE

#include "log_device_impl.h"

class LogDeviceFile : public LogDeviceImpl
{
public:
	LogDeviceFile();
	LogDeviceFile(const TCHAR* strPath, const TCHAR* strFileNamePrefix, const int32 iMaxFileSize = 1<<20);
	~LogDeviceFile();

	void Init(const TCHAR* strPath, const TCHAR* strFileNamePrefix, const int32 iMaxFileSize /* = 1<20 */);
	void LogOutput(TCHAR* strBuffer);
	
private:
	void _FileOpen(const TCHAR* strPath, const TCHAR* strFileName);
	void _FileClose();

private:
	TCHAR m_strFileName[MAX_PATH + 1];
	FILE* m_pFile;
};

#endif
