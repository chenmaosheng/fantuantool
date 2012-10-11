/*****************************************************************************************
	filename:	client_loop.h
	created:	10/08/2012
	author:		chen
	purpose:	help client to control game tick

*****************************************************************************************/

#ifndef _H_CLIENT_LOOP
#define _H_CLIENT_LOOP

#include "client_common.h"
#include <list>

struct ClientCommand;
class ClientLoop
{
public:
	ClientLoop();
	~ClientLoop();

	// initialize the logic loop
	int32 Init();
	// destroy the logic loop
	void Destroy();

	// start loop
	int32 Start();
	// stop loop
	void Stop();
	void PushCommand(ClientCommand*);

private:
	DWORD _Loop();
	bool _OnCommand(ClientCommand*);

private:
	// thread run function
	static uint32 WINAPI _ThreadMain(PVOID);

private:
	HANDLE m_hThread;
	HANDLE m_hCommandEvent;
	std::list<ClientCommand*> m_CommandList;
	CRITICAL_SECTION m_csCommandList;
	BOOL m_bQuit;
};

#endif
