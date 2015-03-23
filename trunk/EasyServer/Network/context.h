#ifndef _H_CONTEXT
#define _H_CONTEXT

#include "common.h"

enum
{
	INVALID_OPERATION,
	OPERATION_ACCEPT,		// accept operation
	OPERATION_CONNECT,		// connect operation
	OPERATION_DISCONNECT,	// disconnect operation
	OPERATION_RECV,			// receive data operation
	OPERATION_SEND,			// send data operation
};

#define CTXOFFSET	FIELD_OFFSET(Context, overlapped_)	// overlapped_ offset
#define BUFOFFSET	FIELD_OFFSET(Context, buffer_)		// buffer_ offset

struct Connection;
struct Context
{
	WSAOVERLAPPED	overlapped_;		// overlapped io
	WSABUF			wsabuf_;			// overlapped buf
	int32			operation_type_;	// overlapped operation type
	Connection*		connection_;		// context's connection
	
	char			buffer_[(sizeof(SOCKADDR)+16)*2];		// received buffer, at least save 2 sockaddrs(acceptex needed)

	static Context* CreateContext(int32 iOperationType)
	{
		Context* pContext = (Context*)_aligned_malloc(sizeof(Context)+MAX_BUFFER, MEMORY_ALLOCATION_ALIGNMENT);
		ZeroMemory(&pContext->overlapped_, sizeof(WSAOVERLAPPED));
		pContext->operation_type_ = iOperationType;
		pContext->wsabuf_.buf = (char*)pContext->buffer_;
		return pContext;
	}
};

#endif
