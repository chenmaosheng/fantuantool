#ifndef _H_LOG_DEVICE_IMPL
#define _H_LOG_DEVICE_IMPL

#include "log_device.h"

class LogDeviceImpl : public LogDevice
{
public:
	LogDeviceImpl();
	virtual ~LogDeviceImpl();

	void Start();
	void Stop();
	bool IsRunning() const;
	
protected:
	bool m_bRunning;
};

#endif
