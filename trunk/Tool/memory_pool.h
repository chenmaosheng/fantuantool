#ifndef _H_MEMORY_POOL
#define _H_MEMORY_POOL

#include "common.h"

class MemoryPool
{
public:
	static MemoryPool* GetInstance();
	virtual void Init(uint32 iMin, uint32 iMax) = 0;
	virtual void Destroy() = 0;

	virtual void* Allocate(uint32 iSize) = 0;
	virtual void* Allocate(const char* file, int32 line, uint32 iSize) = 0;
	virtual void Free(void*) = 0;
	virtual void Free(const char* file, int32 line, void*) = 0;
};

#define MEMORY_POOL_INIT(min, max) MemoryPool::GetInstance()->Init(min, max)

#endif
