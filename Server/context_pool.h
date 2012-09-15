#ifndef _H_CONTEXT_POOL
#define _H_CONTEXT_POOL

#include "common.h"

struct Context;
class ContextPool
{
public:
	ContextPool(){}
	~ContextPool(){}

	void Init(uint32 input_buffer_size, uint32 output_buffer_size);
	void Destroy();

	Context* PopInputContext();
	Context* PopOutputContext();
	void PushInputContext(Context*);
	void PushOutputContext(Context*);
	char* PopOutputBuffer();
	void PushOutputBuffer(char* buffer);
	
	static ContextPool* CreateContextPool(uint32 input_buffer_size, uint32 output_buffer_size);
	static void DestroyContextPool(ContextPool*);


public:
	uint32	input_buffer_size_;
	uint32	input_context_count_;
	uint32	output_buffer_size_;
	uint32	output_context_count_;


	SLIST_HEADER input_context_pool_;
	SLIST_HEADER output_context_pool_;
};

#endif
