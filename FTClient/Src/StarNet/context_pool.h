/*****************************************************************************************
	filename:	context_pool.h
	created:	09/27/2012
	author:		chen
	purpose:	create a connection to restore connect from client

*****************************************************************************************/

#ifndef _H_CONTEXT_POOL
#define _H_CONTEXT_POOL

#include "starnet_common.h"

struct Context;
class ContextPool
{
public:
	// initialize context pool with initial input size and output size
	void Init(uint32 input_buffer_size, uint32 output_buffer_size);
	// destroy context pool
	void Destroy();

	// pop a context from input and output pool
	Context* PopInputContext();
	Context* PopOutputContext();
	// push a context into input and output pool
	void PushInputContext(Context*);
	void PushOutputContext(Context*);

	// pop a context, but only return its buffer
	char* PopOutputBuffer();
	// push a context, use its buffer to point to the context
	void PushOutputBuffer(char* buffer);
	
	static ContextPool* CreateContextPool(uint32 input_buffer_size, uint32 output_buffer_size);
	static void DestroyContextPool(ContextPool*);

public:
	uint32	input_buffer_size_;			// input pool size
	uint32	input_context_count_;		// used input count
	uint32	output_buffer_size_;		// output pool size
	uint32	output_context_count_;		// used output count

	SLIST_HEADER input_context_pool_;	// SList of input pool
	SLIST_HEADER output_context_pool_;	// SList of output pool
};

#endif
