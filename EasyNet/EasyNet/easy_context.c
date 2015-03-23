#include "easy_context.h"

#ifdef __cplusplus
extern "C" {
#endif

EasyContext* CreateContext(int32 iOperationType)
{
	EasyContext* pContext = (EasyContext*)_aligned_malloc(sizeof(EasyContext)+MAX_BUFFER, MEMORY_ALLOCATION_ALIGNMENT);
	ZeroMemory(&pContext->overlapped_, sizeof(WSAOVERLAPPED));
	pContext->operation_type_ = iOperationType;
	pContext->wsabuf_.buf = (char*)pContext->buffer_;
	return pContext;
}

#ifdef __cplusplus
}
#endif
