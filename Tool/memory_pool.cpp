#include "memory_pool.h"
#include "singleton.h"

class MemoryPoolImpl : public MemoryPool,
					   public Singleton<MemoryPoolImpl>
{
public:
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
	uint32 m_iCount;
};

MemoryPool* MemoryPool::GetInstance()
{
	return MemoryPoolImpl::Instance();
}

void MemoryPoolImpl::Init(uint32 iMin, uint32 iMax)
{
	//m_iMinPower = NUMBER_POWER(iMin);
	//m_iMaxPower = NUMBER_POWER(iMax);
	m_iCount = 0;
}

MemoryPoolImpl::~MemoryPoolImpl()
{
}

void* MemoryPoolImpl::Allocate(uint32 iSize)
{
	return Allocate(NULL, 0, iSize);
}

void MemoryPoolImpl::Free(void* p)
{
	Free(NULL, 0, p);
}
