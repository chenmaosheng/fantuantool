#include "log_device_file.h"
#include <cstdio>
#include <io.h>

LogDeviceFile::LogDeviceFile(const TCHAR* strPath, const TCHAR* strFileNamePrefix, const int32 iMaxFileSize)
: m_pFile(NULL)
{
	ZeroMemory(m_strFileName, sizeof(m_strFileName));

	SYSTEMTIME now;
	GetLocalTime(&now);

	_FileClose();

	_stprintf_s(m_strFileName, _T("%s_%04d%02d%02d_%d.log"), strFileNamePrefix, now.wYear, now.wMonth, now.wDay, now.wHour*3600+now.wMinute*60+now.wSecond);
	_FileOpen(strPath, m_strFileName);
}

LogDeviceFile::~LogDeviceFile()
{
	_FileClose();
}

void LogDeviceFile::LogOutput(TCHAR *strBuffer)
{
	if (!m_pFile)
	{
		return;
	}

	fprintf_s(m_pFile, "%ls", strBuffer);
	fflush(m_pFile);
}

void LogDeviceFile::_FileOpen(const TCHAR *strPath, const TCHAR *strFileName)
{
	int32 iRet = 0;
	TCHAR fullFileName[1024] = {0};

	// check if path directory exists
	if (_waccess(strPath, 0) == -1)
	{
		iRet = _wmkdir(strPath);
		if (iRet != 0)
		{
			return;
		}
	}

	_stprintf_s(fullFileName, _T("%s/%s"), strPath, strFileName);
	iRet = _wfopen_s(&m_pFile, fullFileName, _T("a+"));
	if (iRet != 0)
	{
		return;
	}
}

void LogDeviceFile::_FileClose()
{
	if (m_pFile)
	{
		fclose(m_pFile);
	}
}

