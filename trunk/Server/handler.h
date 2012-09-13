#ifndef _H_HANDLER
#define _H_HANDLER

#include "common.h"

class Connection;
class Handler
{
public:
	virtual bool	OnConnection(ConnID iConnID) = 0;
	virtual void	OnDisconnect(ConnID iConnID) = 0;
	virtual void	OnData(ConnID iConnID, uint16 length, LPBYTE buffer) = 0;
	virtual void	OnConnectFailed(void) = 0;
};

#endif
