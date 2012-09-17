#ifndef _H_LOG
#define _H_LOG

#include "log_device.h"
#include <vector>

class SingleBuffer;
class Log
{
public:
	// 每次output的大小
	const static uint16 m_iDelayMaxSize = 128;
	const static uint16 m_iMaxSize = 65535;

	Log();
	~Log();

	static Log& Instance()
	{
		static Log log;
		return log;
	}

	void Start();
	void AddLogDevice(LogDevice*);
	bool Push(TCHAR* strBuffer, uint16 iLength);

private:
	static uint32 WINAPI _LogOutput(PVOID);
	void _Tick();
	void _Output(TCHAR* strBuffer);

private:
	TCHAR m_strBuffer[m_iMaxSize];
	SingleBuffer* m_pBuffer;
	std::vector<LogDevice*> m_vLogDeviceList;
	CRITICAL_SECTION m_cs;
	HANDLE m_hOutputEvent;
	HANDLE m_hThread;
};

#endif
