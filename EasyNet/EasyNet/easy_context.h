#ifndef _H_EASY_CONTEXT
#define _H_EASY_CONTEXT

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

enum
{
	INVALID_OPERATION,
	OPERATION_ACCEPT,		// accept operation
	OPERATION_CONNECT,		// connect operation
	OPERATION_DISCONNECT,	// disconnect operation
	OPERATION_RECV,			// receive data operation
	OPERATION_SEND,			// send data operation
};

#define CTXOFFSET	FIELD_OFFSET(EasyContext, overlapped_)	// overlapped_ offset
#define BUFOFFSET	FIELD_OFFSET(EasyContext, buffer_)		// buffer_ offset

struct _EasyConnection;
typedef struct _EasyContext
{
	WSAOVERLAPPED				overlapped_;		// overlapped io
	WSABUF						wsabuf_;			// overlapped buf
	int32						operation_type_;	// overlapped operation type
	struct _EasyConnection*		connection_;		// context's connection
	char						buffer_[(sizeof(SOCKADDR)+16)*2];		// received buffer, at least save 2 sockaddrs(acceptex needed)
}EasyContext;

EasyContext* CreateContext(int32 iOperationType);

#ifdef __cplusplus
}
#endif

#endif
