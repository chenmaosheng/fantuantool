#include "easy_buffer.h"

#ifdef __cplusplus
extern "C" {
#endif

EasyBuffer*	CreateEasyBuffer(size_t max_size)
{
	EasyBuffer* pBuffer = (EasyBuffer*)_aligned_malloc(sizeof(EasyBuffer), MEMORY_ALLOCATION_ALIGNMENT);
	pBuffer->data_ = _aligned_malloc(max_size, MEMORY_ALLOCATION_ALIGNMENT);
	memset(pBuffer->data_, 0, max_size);
	pBuffer->tail_ = pBuffer->data_;
	pBuffer->size_ = max_size;
	pBuffer->curr_size_ = 0;

	return pBuffer;
}

void		DestroyEasyBuffer(EasyBuffer* pBuffer)
{
	_aligned_free(pBuffer->data_);
	_aligned_free(pBuffer);
}

BOOL		PushToEasyBuffer(EasyBuffer* pBuffer, const void* pItem, size_t iLength)
{
	if (pBuffer->curr_size_ + iLength >= pBuffer->size_)
	{
		return FALSE;
	}

	memcpy(pBuffer->tail_, pItem, iLength);
	pBuffer->tail_ = (BYTE*)pBuffer->tail_ + iLength;
	pBuffer->curr_size_ += iLength;

	return TRUE;
}

size_t		PopFromEasyBuffer(EasyBuffer* pBuffer, void* pItem, size_t iLength)
{
	if (!pItem)
	{
		return 0;
	}

	if (pBuffer->curr_size_ < iLength)
	{
		iLength = pBuffer->curr_size_;
	}

	pBuffer->tail_ = (BYTE*)pBuffer->tail_ - iLength;
	memcpy(pItem, pBuffer->tail_, iLength);
	pBuffer->curr_size_ -= iLength;

	return iLength;
}

#ifdef __cplusplus
}
#endif
