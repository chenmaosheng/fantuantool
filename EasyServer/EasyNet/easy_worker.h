#ifndef _H_EASY_WORKER
#define _H_EASY_WORKER

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _EasyWorker {
	HANDLE  iocp_;			// binded iocp handle
	uint32  thread_count_;	// the number of threads
}EasyWorker;

// initialize worker with several threads
extern EasyWorker*	CreateWorker(uint32 iThreadCount);
// destroy worker
extern void			DestroyWorker(EasyWorker*);

#ifdef __cplusplus
}
#endif

#endif
