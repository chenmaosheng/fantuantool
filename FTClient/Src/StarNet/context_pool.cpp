#include "context_pool.h"
#include "context.h"

void ContextPool::Init(uint32 input_buffer_size, uint32 output_buffer_size)
{
	input_buffer_size_ = input_buffer_size;
	output_buffer_size_ = output_buffer_size;
	input_context_count_ = 0;
	output_context_count_ = 0;

	// initialize each Slist
	InitializeSListHead(&input_context_pool_);
	InitializeSListHead(&output_context_pool_);

	SN_LOG_STT(_T("Initialize context pool success"));
}

void ContextPool::Destroy()
{
	while (QueryDepthSList(&input_context_pool_) != input_context_count_)
	{
		Sleep(100);
	}

	while (QueryDepthSList(&input_context_pool_))
	{
		_aligned_free(InterlockedPopEntrySList(&input_context_pool_));
	}

	while (QueryDepthSList(&output_context_pool_) != output_context_count_)
	{
		Sleep(100);
	}

	while (QueryDepthSList(&output_context_pool_))
	{
		_aligned_free(InterlockedPopEntrySList(&output_context_pool_));
	}

	SN_LOG_STT(_T("Destroy context pool success"));
}

Context* ContextPool::PopInputContext()
{
	Context* pContext = (Context*)InterlockedPopEntrySList(&input_context_pool_);
	if (!pContext)
	{
		pContext = (Context*)_aligned_malloc(sizeof(Context)+input_buffer_size_, MEMORY_ALLOCATION_ALIGNMENT);
		_ASSERT(pContext);
		if (!pContext)
		{
			SN_LOG_ERR(_T("Allocate context failed, err=%d"), GetLastError());
			return NULL;
		}

		ZeroMemory(&pContext->overlapped_, sizeof(WSAOVERLAPPED));
		pContext->operation_type_ = OPERATION_RECV;
		pContext->context_pool_ = this;
		pContext->wsabuf_.buf = (char*)pContext->buffer_;
		InterlockedIncrement((LONG*)&input_context_count_);
	}

	return pContext;
}

Context* ContextPool::PopOutputContext()
{
	Context* pContext = (Context*)InterlockedPopEntrySList(&output_context_pool_);
	if (!pContext)
	{
		pContext = (Context*)_aligned_malloc(sizeof(Context)+output_buffer_size_, MEMORY_ALLOCATION_ALIGNMENT);
		_ASSERT(pContext);
		if (!pContext)
		{
			SN_LOG_ERR(_T("Allocate context failed, err=%d"), GetLastError());
			return NULL;
		}

		ZeroMemory(&pContext->overlapped_, sizeof(WSAOVERLAPPED));
		pContext->operation_type_ = OPERATION_SEND;
		pContext->context_pool_ = this;
		pContext->wsabuf_.buf = (char*)pContext->buffer_;
		InterlockedIncrement((LONG*)&output_context_count_);
	}

	return pContext;
}

void ContextPool::PushInputContext(Context* pContext)
{
	InterlockedPushEntrySList(&input_context_pool_, pContext);
}

void ContextPool::PushOutputContext(Context* pContext)
{
	InterlockedPushEntrySList(&output_context_pool_, pContext);
}

char* ContextPool::PopOutputBuffer()
{
	Context* pContext = PopOutputContext();
	_ASSERT(pContext);
	if (pContext)
	{
		return pContext->buffer_;
	}

	SN_LOG_ERR(_T("Pop a buffer from pool failed, err=%d"), GetLastError());

	return NULL;
}

void ContextPool::PushOutputBuffer(char* buffer)
{
	PushOutputContext((Context*)((char*)buffer - BUFOFFSET));
}

ContextPool* ContextPool::CreateContextPool(uint32 input_buffer_size, uint32 output_buffer_size)
{
	ContextPool* pContextPool = (ContextPool*)_aligned_malloc(sizeof(ContextPool), MEMORY_ALLOCATION_ALIGNMENT);
	if (pContextPool)
	{
		pContextPool->Init(input_buffer_size, output_buffer_size);
	}

	return pContextPool;
}

void ContextPool::DestroyContextPool(ContextPool* pContextPool)
{
	pContextPool->Destroy();
	_aligned_free(pContextPool);
}

