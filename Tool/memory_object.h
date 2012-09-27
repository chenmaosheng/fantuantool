/*****************************************************************************************
	filename:	memory_object.h
	created:	09/27/2012
	author:		chen
	purpose:	all object want to use memory pool need to inherit from this memory object

*****************************************************************************************/

#ifndef _H_MEMORY_OBJECT
#define _H_MEMORY_OBJECT

#include "memory_pool.h"

struct MemoryObject
{
	virtual ~MemoryObject(){}

	static void* operator new(size_t size);
	static void* operator new[](size_t size);
	static void operator delete(void* p);
	static void operator delete[](void* p);

	void* operator new(size_t size, MemoryPool*);
	void operator delete(void* p, MemoryPool*);

	// use this function to do actual allocation
	template<typename T>
	static T* Allocate();
	
	// use this function to free existing memory object
	static void Free(MemoryObject*);
};

template<typename T>
__forceinline T* MemoryObject::Allocate()
{
	return new (MemoryPool::GetInstance()) T();
}

__forceinline void MemoryObject::Free(MemoryObject* pObject)
{
	pObject->~MemoryObject();
	operator delete(pObject, MemoryPool::GetInstance());
}

#define FT_NEW(type) MemoryObject::Allocate<type>()
#define FT_DELETE(obj) MemoryObject::Free(obj)

#endif
