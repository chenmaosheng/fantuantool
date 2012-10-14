#include "memory_pool.h"
#include "singleton.h"

class MemoryPoolImpl : public MemoryPool,
					   public Singleton<MemoryPoolImpl>
{
public:
	// one node points to an allocated memory, use SList to control push and pop
	struct Node : SLIST_ENTRY
	{
		uint8 m_iIndex;	// the index of this node in the SList
		char m_Buf[1];	// point to the first byte of buffer
	};

	~MemoryPoolImpl();

	// Initialize memory pool, min is the minimum bytes of allocate object, max is the maximum bytes of allocate object
	void Init(uint32 iMin, uint32 iMax);
	void Destroy();

	void* Allocate(uint32 iSize);
	void* Allocate(const char* file, int32 line, uint32 iSize);
	void Free(void*);
	void Free(const char* file, int32 line, void*);

private:
	uint32 m_iMinPower;			// the minimum 2nd power
	uint32 m_iMaxPower;			// the maximum 2nd power
	volatile LONG m_iCount;		// the count of all allocated object
	SLIST_HEADER m_Headers[32];	// all possible SLists
	uint32 m_iHeaderCount;		// the number of SList
};

MemoryPool* MemoryPool::GetInstance()
{
	return MemoryPoolImpl::Instance();
}

void MemoryPoolImpl::Init(uint32 iMin, uint32 iMax)
{
	// get min and max, and initialize all SList
	m_iMinPower = NumberPower(iMin);
	m_iMaxPower = NumberPower(iMax);
	m_iCount = 0;
	m_iHeaderCount = m_iMaxPower - m_iMinPower + 1;
	for (uint32 i = 0; i < m_iHeaderCount; ++i)
	{
		InitializeSListHead(&m_Headers[i]);
	}
}

void MemoryPoolImpl::Destroy()
{

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
		
	_ASSERT(m_iCount == 0 && _T("m_iCount != 0"));
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
	// get the 2nd power of this size
	uint8 iPower = NumberPower(iSize);
	_ASSERT(iPower <= m_iMaxPower);
	if (iPower > m_iMaxPower)
	{
		return NULL;
	}

	iPower = iPower < m_iMinPower ? m_iMinPower : iPower;
	uint8 iIndex = iPower - m_iMinPower;
	// get a node from related SList
	Node* pNode = (Node*)InterlockedPopEntrySList(&m_Headers[iIndex]);
	if (!pNode)
	{
		pNode = (Node*)_aligned_malloc(sizeof(Node)+(1<<iPower), MEMORY_ALLOCATION_ALIGNMENT);
		pNode->m_iIndex = iIndex;
		InterlockedIncrement(&m_iCount);
	}

	// notice that the node has one byte to restore index
	return pNode+1;
}

void MemoryPoolImpl::Free(const char *file, int32 line, void* p)
{
	// put node into related SList
	Node* pNode = (Node*)p - 1;
	uint8 iPower = pNode->m_iIndex + m_iMinPower;
	InterlockedPushEntrySList(&m_Headers[pNode->m_iIndex], pNode);
}