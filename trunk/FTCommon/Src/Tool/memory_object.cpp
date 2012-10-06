#include "memory_object.h"

void* MemoryObject::operator new(size_t size, MemoryPool* pPool)
{
	return pPool->Allocate(size);
}

void MemoryObject::operator delete(void* p, MemoryPool* pPool)
{
	pPool->Free(p);
}

void* MemoryObject::operator new(size_t)
{
	return NULL;
}

void* MemoryObject::operator new[](size_t)
{
	return NULL;
}

void MemoryObject::operator delete(void*)
{
	return;
}

void MemoryObject::operator delete[](void*)
{
	return;
}