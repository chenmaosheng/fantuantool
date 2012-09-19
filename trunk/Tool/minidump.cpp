#include "minidump.h"
#include <Psapi.h>
#include <DbgHelp.h>

TCHAR Minidump::m_strDumpPath[MAX_PATH] = {0};
HANDLE Minidump::m_hProcess = NULL;
DWORD Minidump::m_dwProcessId = 0;

void Minidump::Init(TCHAR *strPath)
{
	DWORD dwRet = 0;
	TCHAR strModuleName[MAX_PATH] = {0};

	dwRet = GetModuleBaseName(GetCurrentProcess(), NULL, strModuleName, sizeof(strModuleName)/sizeof(TCHAR));
	if (dwRet == 0)
	{
		wcscpy_s(strModuleName, sizeof(strModuleName)/sizeof(TCHAR), _T("Fantuan"));
	}

	wcscpy_s(m_strDumpPath, sizeof(m_strDumpPath)/sizeof(TCHAR), strPath);
	wcscat_s(m_strDumpPath, sizeof(m_strDumpPath)/sizeof(TCHAR), _T("\\"));
	wcscat_s(m_strDumpPath, sizeof(m_strDumpPath)/sizeof(TCHAR), strModuleName);

	SYSTEMTIME now;
	GetLocalTime(&now);
	_stprintf_s(m_strDumpPath, sizeof(m_strDumpPath)/sizeof(TCHAR), _T("%s_%02d.%02d.%02d %02d.%02d.%02d.dmp"),
		m_strDumpPath, now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond);

	m_hProcess = GetCurrentProcess();
	m_dwProcessId = GetCurrentProcessId();

	SetUnhandledExceptionFilter(_UnhandledExceptionFilter);
	_set_invalid_parameter_handler(_InvalidParameterHandler);
	_set_purecall_handler(_UnHandledPurecallHandler);
}

void Minidump::_Create(_EXCEPTION_POINTERS *pExceptionPointers)
{
	HANDLE hDump;
	BOOL bRet = 0;
	MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;

	exceptionInfo.ThreadId = GetCurrentThreadId();
	exceptionInfo.ExceptionPointers = pExceptionPointers;
	exceptionInfo.ClientPointers = FALSE;

	hDump = CreateFile(m_strDumpPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, NULL);
	if (hDump == INVALID_HANDLE_VALUE)
	{
		return;
	}

	bRet = MiniDumpWriteDump(m_hProcess, m_dwProcessId, hDump, MiniDumpWithFullMemory, pExceptionPointers?&exceptionInfo:NULL, NULL, NULL);
	if (!bRet)
	{
		bRet = MiniDumpWriteDump(m_hProcess, m_dwProcessId, hDump, MiniDumpNormal, pExceptionPointers?&exceptionInfo:NULL, NULL, NULL);
		if (!bRet)
		{
			return;
		}
	}

	CloseHandle(hDump);
}

LONG WINAPI Minidump::_UnhandledExceptionFilter( __in struct _EXCEPTION_POINTERS *ExceptionInfo )
{
	_Create(ExceptionInfo);
	return EXCEPTION_EXECUTE_HANDLER;
}

void Minidump::_InvalidParameterHandler(const TCHAR* strExpression, const TCHAR* strFunction, const TCHAR* strFileName, uint32 iLine, uintptr_t iPreserved)
{
	_Create(NULL);
	exit(0);
}

void Minidump::_UnHandledPurecallHandler()
{
	_Create(NULL);
	exit(0);
}