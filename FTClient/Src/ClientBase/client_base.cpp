#include "client_base.h"
#include "worker.h"
#include "minidump.h"
#include "simplenet.h"

ClientBase::ClientBase()
{
	m_pWorker = NULL;
	m_pLogSystem = NULL;
}

ClientBase::~ClientBase()
{
}

int32 ClientBase::Init()
{
	int32 iRet = 0;

	// initialize minidump
	Minidump::Init(_T("log"));

	// set the min and max of memory pool object
	MEMORY_POOL_INIT(MEMORY_OBJECT_MIN, MEMORY_OBJECT_MAX);

	iRet = SimpleNet::Init();
	if (iRet != 0)
	{
		LOG_ERR(LOG_SERVER, _T("Initialize SimpleNet failed"));
		return -3;
	}



	return 0;
}

void ClientBase::Destroy()
{
	SimpleNet::Destroy();
}

void ClientBase::Login(const char *strHost, const char *strToken)
{
}

