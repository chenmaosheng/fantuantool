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

	// get module name in order to create correct dump file name
	dwRet = GetModuleBaseName(GetCurrentProcess(), NULL, strModuleName, _countof(strModuleName));
	if (dwRet == 0)
	{
		wcscpy_s(strModuleName, _countof(strModuleName), _T("Fantuan"));
	}

	wcscpy_s(m_strDumpPath, _countof(m_strDumpPath), strPath);
	wcscat_s(m_strDumpPath, _countof(m_strDumpPath), _T("\\"));
	wcscat_s(m_strDumpPath, _countof(m_strDumpPath), strModuleName);

	SYSTEMTIME now;
	GetLocalTime(&now);
	_stprintf_s(m_strDumpPath, _countof(m_strDumpPath), _T("%s_%02d.%02d.%02d %02d.%02d.%02d.dmp"),
		m_strDumpPath, now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond);

	// record process handle and id
	m_hProcess = GetCurrentProcess();
	m_dwProcessId = GetCurrentProcessId();

	// set all possible dump filter
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

	// write dump file with full memory
	bRet = MiniDumpWriteDump(m_hProcess, m_dwProcessId, hDump, MiniDumpWithFullMemory, pExceptionPointers?&exceptionInfo:NULL, NULL, NULL);
	if (!bRet)
	{
		// if failed, try to write dump with normal memory
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