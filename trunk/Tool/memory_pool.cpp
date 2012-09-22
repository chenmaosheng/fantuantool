#include "memory_pool.h"
#include "singleton.h"
#include "util.h"

class MemoryPoolImpl : public MemoryPool,
					   public Singleton<MemoryPoolImpl>
{
public:
	struct Node : SLIST_ENTRY
	{
		uint8 m_iIndex;
		char m_Buf[1];
	};

	~MemoryPoolImpl();

	void Init(uint32 iMin, uint32 iMax);
	void Destroy();

	void* Allocate(uint32 iSize);
	void* Allocate(const char* file, int32 line, uint32 iSize);
	void Free(void*);
	void Free(const char* file, int32 line, void*);

private:
	uint32 m_iMinPower;
	uint32 m_iMaxPower;
	volatile LONG m_iCount;
	SLIST_HEADER m_Headers[32];
	uint32 m_iHeaderCount;
};

MemoryPool* MemoryPool::GetInstance()
{
	return MemoryPoolImpl::Instance();
}

void MemoryPoolImpl::Init(uint32 iMin, uint32 iMax)
{
	m_iMinPower = NumberPower(iMin);
	m_iMaxPower = NumberPower(iMax);
	m_iCount = 0;
	m_iHeaderCount = m_iMaxPower - m_iMinPower + 1;
	for (uint32 i = 0; i < m_iHeaderCount; ++i)
	{
		InitializeSListHead(&m_Headers[i]);
	}
}

MemoryPoolImpl::~MemoryPoolImpl()
{
	Node* pNode = NULL;
	for (uint32 i = 0; i < m_iHeaderCount; ++i)
	{
		while (QueryDepthSList(&m_Headers[i]))
		{
			m_iCount--;
			_aligned_free(InterlockedPopEntrySList(&m_Headers[i]));
		}
	}

	if (m_iCount != 0)
	{
		// todo: fatal error
	}
}

void* MemoryPoolImpl::Allocate(uint32 iSize)
{
	return Allocate(NULL, 0, iSize);
}

void MemoryPoolImpl::Free(void* p)
{
	Free(NULL, 0, p);
}

void* MemoryPoolImpl::Allocate(const char *file, int32 line, uint32 iSize)
{
	uint8 iPower = NumberPower(iSize);
	if (iPower > m_iMaxPower)
	{
		return NULL;
	}

	iPower = iPower < m_iMinPower ? m_iMinPower : iPower;
	uint8 iIndex = iPower - m_iMinPower;
	Node* pNode = (Node*)InterlockedPopEntrySList(&m_Headers[iIndex]);
	if (!pNode)
	{
		pNode = (Node*)_aligned_malloc(sizeof(Node)+(1<<iPower), MEMORY_ALLOCATION_ALIGNMENT);
		pNode->m_iIndex = iIndex;
		InterlockedIncrement(&m_iCount);
	}

	return pNode+1;
}

void MemoryPoolImpl::Free(const char *file, int32 line, void* p)
{
	Node* pNode = (Node*)p - 1;
	uint8 iPower = pNode->m_iIndex + m_iMinPower;
	InterlockedPushEntrySList(&m_Headers[pNode->m_iIndex], pNode);
}