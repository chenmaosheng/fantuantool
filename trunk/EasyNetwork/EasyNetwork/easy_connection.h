#ifndef _H_EASY_CONNECTION
#define _H_EASY_CONNECTION

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _EasyAcceptor;
typedef struct _EasyConnection
{
	SOCKET					socket_;        
	char					buffer_[1024];
	int32					buffer_size_;
	struct _EasyAcceptor*	acceptor_;				// related acceptor
	void*					client_;				// pointer from app layer    
}EasyConnection;

// static function to create and close
extern EasyConnection*		CreateConnection(struct _EasyAcceptor* pAcceptor);

#ifdef __cplusplus
}
#endif

#endif
