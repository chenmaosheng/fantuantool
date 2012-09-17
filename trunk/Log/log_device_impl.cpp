#include "log_device_impl.h"

LogDeviceImpl::LogDeviceImpl()
{
	m_bRunning = false;
}

LogDeviceImpl::~LogDeviceImpl()
{
}

void LogDeviceImpl::Start()
{
	m_bRunning = true;
}

void LogDeviceImpl::Stop()
{
	m_bRunning = false;
}

bool LogDeviceImpl::IsRunning() const
{
	return m_bRunning;
}