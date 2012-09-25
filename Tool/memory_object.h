#ifndef _H_MEMORY_OBJECT
#define _H_MEMORY_OBJECT

#include "memory_pool.h"

class MemoryObject
{
public:
	virtual ~MemoryObject(){}

	static void* operator new(size_t size);
	static void* operator new[](size_t size);
	static void operator delete(void* p);
	static void operator delete[](void* p);

	void* operator new(size_t size, MemoryPool*);
	void operator delete(void* p, MemoryPool*);

	template<typename T>
	static T* Allocate();
	
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
