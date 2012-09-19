#ifndef _H_MINIDUMP
#define _H_MINIDUMP

#include "common.h"

class Minidump
{
public:
	static void Init(TCHAR* strPath);

private:
	static void _Create(_EXCEPTION_POINTERS* pExceptionPointers);
	static LONG WINAPI _UnhandledExceptionFilter( __in struct _EXCEPTION_POINTERS *ExceptionInfo );
	static void _InvalidParameterHandler(const TCHAR* strExpression, const TCHAR* strFunction, const TCHAR* strFileName, uint32 iLine, uintptr_t iPreserved);
	static void _UnHandledPurecallHandler();

private:
	static TCHAR m_strDumpPath[MAX_PATH];
	static HANDLE m_hProcess;
	static DWORD m_dwProcessId;
};

#endif
