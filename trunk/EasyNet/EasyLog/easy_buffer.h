#ifndef _H_EASY_BUFFER
#define _H_EASY_BUFFER

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _EasyBuffer
{
	void*			data_;
	void* volatile	tail_;
	size_t			size_;
	size_t			curr_size_;
}EasyBuffer;

EasyBuffer*	CreateEasyBuffer(size_t max_size);
void		DestroyEasyBuffer(EasyBuffer*);
BOOL		PushToEasyBuffer(EasyBuffer*, const void*, size_t);
size_t		PopFromEasyBuffer(EasyBuffer*, void*, size_t);

#ifdef __cplusplus
}
#endif

#endif
