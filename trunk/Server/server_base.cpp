#include "server_base.h"
#include "connection.h"
#include "worker.h"
#include "acceptor.h"
#include "context_pool.h"

ServerBase::ServerBase()
{
	worker_ = NULL;
	acceptor_ = NULL;
	context_pool_= NULL;
}

ServerBase::~ServerBase()
{
}

int32 ServerBase::InitAcceptor(uint32 ip, uint16 port, Handler *pHandler)
{
	worker_ = Worker::CreateWorker();
	if (!worker_)
	{
		return -1;
	}

	context_pool_ = ContextPool::CreateContextPool(MAX_INPUT_BUFFER, MAX_OUTPUT_BUFFER);
	if (!context_pool_)
	{
		return -2;
	}

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons(port);
	acceptor_ = Acceptor::CreateAcceptor(&addr, worker_, context_pool_, pHandler);
	if (!acceptor_)
	{
		return -3;
	}

	return 0;
}

void ServerBase::DestroyAcceptor()
{
	if (worker_)
	{
		Worker::DestroyWorker(worker_);
		worker_ = NULL;
	}

	if (context_pool_)
	{
		ContextPool::DestroyContextPool(context_pool_);
		context_pool_ = NULL;
	}

	if (acceptor_)
	{
		Acceptor::DestroyAcceptor(acceptor_);
		acceptor_ = NULL;
	}
}

void ServerBase::StartAcceptor()
{
	acceptor_->Start();
}

void ServerBase::StopAcceptor()
{
}

ContextPool* ServerBase::GetContextPool()
{
	return context_pool_;
}
