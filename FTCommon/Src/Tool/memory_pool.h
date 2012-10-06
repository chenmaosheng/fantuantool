/*****************************************************************************************
	filename:	memory_pool.h
	created:	09/27/2012
	author:		chen
	purpose:	maintain all objects allocated by memory object

*****************************************************************************************/

#ifndef _H_MEMORY_POOL
#define _H_MEMORY_POOL

#include "common.h"

class MemoryPool
{
public:
	static MemoryPool* GetInstance();

	// Initialize memory pool, min is the minimum bytes of allocate object, max is the maximum bytes of allocate object
	virtual void Init(uint32 iMin, uint32 iMax) = 0;
	
	// destroy the whole memory pool
	virtual void Destroy() = 0;

	// allocate a certain size of object
	virtual void* Allocate(uint32 iSize) = 0;
	virtual void* Allocate(const char* file, int32 line, uint32 iSize) = 0;

	// free an object which must be allocated by memory pool, otherwise there will be some problem
	virtual void Free(void*) = 0;
	virtual void Free(const char* file, int32 line, void*) = 0;
};

#define MEMORY_POOL_INIT(min, max) MemoryPool::GetInstance()->Init(min, max)

#endif
