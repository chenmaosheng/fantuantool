#include "easy_net.h"

#ifdef __cplusplus
extern "C" {
#endif

int32 InitNet()
{
	WSADATA wsd;
	// start WSA
	if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
	{
		LOG_ERR(_T("WSAStartup failed"));
		return -1;
	}

	LOG_STT(_T("Intialize Network success"));
	return 0;
}

void DestroyNet()
{
	WSACleanup();
}

#ifdef __cplusplus
}
#endif