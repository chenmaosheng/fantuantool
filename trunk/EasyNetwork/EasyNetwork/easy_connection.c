#include "easy_connection.h"
#include "easy_acceptor.h"
#include "easy_net.h"

#ifdef __cplusplus
extern "C" {
#endif

EasyConnection* CreateConnection(EasyAcceptor* pAcceptor)
{
	EasyConnection* pConnection = (EasyConnection*)_aligned_malloc(sizeof(EasyConnection), MEMORY_ALLOCATION_ALIGNMENT);
	pConnection->buffer_[0] = '\0';
	pConnection->buffer_size_ = 0;
	pConnection->acceptor_ = pAcceptor;
	
	return pConnection;
}

#ifdef __cplusplus
}
#endif
