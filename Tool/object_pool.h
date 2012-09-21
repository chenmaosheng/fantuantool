#ifndef _H_OBJECT_POOL
#define _H_OBJECT_POOL

#include "common.h"
#include <list>

template<typename T>
class ObjectPool
{
public:
	ObjectPool(uint16 iObjectMax);
	~ObjectPool();

	T* Allocate();
	void Free(T*);

private:
	std::list<T*> m_ObjectList;
	uint16 m_iMaxSize;
	uint16 m_iUsedSize;

	CRITICAL_SECTION m_csObject;
};

template<typename T>
ObjectPool<T>::ObjectPool(uint16 iObjectMax)
{
	InitializeCriticalSection(&m_csObject);

	m_iMaxSize = iObjectMax;
	m_iUsedSize = 0;
}

template<typename T>
ObjectPool<T>::~ObjectPool()
{
	EnterCriticalSection(&m_csObject);
	for (std::list<T*>::iterator it = m_ObjectList.begin(); it != m_ObjectList.end(); ++it)
	{
		SAFE_DELETE(*it);
	}
	LeaveCriticalSection(&m_csObject);
	DeleteCriticalSection(&m_csObject);
}

template<typename T>
T* ObjectPool<T>::Allocate()
{
	T* pObject = NULL;
	EnterCriticalSection(&m_csObject);
	if (m_iMaxSize > m_iUsedSize)
	{
		if (m_ObjectList.empty())
		{
			pObject = new T();
		}
		else
		{
			pObject = m_ObjectList.front();
			m_ObjectList.pop_front();
		}

		++m_iUsedSize;
	}

	LeaveCriticalSection(&m_csObject);
	return pObject;
}

template<typename T>
void ObjectPool<T>::Free(T* pObj)
{
	EnterCriticalSection(&m_csObject);

	m_ObjectList.push_back(pObj);
	++m_iUsedSize;

	LeaveCriticalSection(&m_csObject);
}

#endif
