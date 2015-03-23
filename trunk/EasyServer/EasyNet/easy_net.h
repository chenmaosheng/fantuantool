#ifndef _H_EASY_NET
#define _H_EASY_NET

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

int32 InitNet();
void DestroyNet();

extern LPFN_ACCEPTEX acceptex_;		// asynchorous accept function address
extern LPFN_CONNECTEX connectex_;	// asynchorous connect function address
extern LPFN_DISCONNECTEX disconnectex_; // asynchorous disconnect function address
extern LPFN_GETACCEPTEXSOCKADDRS getacceptexsockaddrs_;	// asynchorous getsockaddr function address

#ifdef __cplusplus
}
#endif

#endif
