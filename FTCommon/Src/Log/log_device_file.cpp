#include "log_device_file.h"
#include <cstdio>
#include <io.h>

LogDeviceFile::LogDeviceFile()
: m_hFile(NULL)
{
	ZeroMemory(m_strFileName, sizeof(m_strFileName));
}

LogDeviceFile::LogDeviceFile(const TCHAR* strPath, const TCHAR* strFileNamePrefix, const int32 iMaxFileSize)
: m_hFile(NULL)
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

void LogDeviceFile::Init(const TCHAR* strPath, const TCHAR* strFileNamePrefix, const int32 iMaxFileSize )
{
	SYSTEMTIME now;
	GetLocalTime(&now);

	_FileClose();

	_stprintf_s(m_strFileName, _T("%s_%04d%02d%02d_%d.log"), strFileNamePrefix, now.wYear, now.wMonth, now.wDay, now.wHour*3600+now.wMinute*60+now.wSecond);
	_FileOpen(strPath, m_strFileName);
}

void LogDeviceFile::LogOutput(TCHAR *strBuffer, uint16 iCount)
{
	DWORD dwBytesWritten = 0;

	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		return;
	}

	WriteFile(m_hFile, strBuffer, sizeof(TCHAR)*iCount, &dwBytesWritten, NULL);
	FlushFileBuffers(m_hFile);
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
	m_hFile = CreateFile(fullFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, 0, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		_ASSERT(false);
	}
}

void LogDeviceFile::_FileClose()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hFile);
	}
}

