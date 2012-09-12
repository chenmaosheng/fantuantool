#ifndef _H_CONTEXT
#define _H_CONTEXT

#include <winsock2.h>

enum
{
	INVALID_OPERATION,
	OPERATION_ACCEPT,		// accept operation
	OPERATION_CONNECT,		// connect operation
	OPERATION_DISCONNECT,	// disconnect operation
	OPERATION_RECV,			// receive data operation
	OPERATION_SEND,			// send data operation
};

#define CTXOFFSET	FIELD_OFFSET(Context, overlapped_)	// overlapped_��������ƫ��

struct Connection;
struct Context
{
	WSAOVERLAPPED	overlapped_;		// overlapped io
	WSABUF			wsabuf_;			// overlapped buf
	int				operation_type_;	// overlapped operation type
	Connection*		connection_;		// context��Ӧ��connection	

	char			buffer_[1024];		// �յ���buffer

	void Reset()
	{
		ZeroMemory(&overlapped_, sizeof(WSAOVERLAPPED));
		wsabuf_.buf = buffer_;
	}
};

#endif