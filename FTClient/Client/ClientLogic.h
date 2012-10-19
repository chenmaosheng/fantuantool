/*****************************************************************************************
	filename:	ClientLogic.h
	created:	10/11/2012
	author:		chen
	purpose:	separate logic from client ui

*****************************************************************************************/

#ifndef _H_CLIENT_LOGIC
#define _H_CLIENT_LOGIC

#include "client_base.h"

class ClientLogic
{
public:
	ClientLogic(ClientBase* pClientBase);
	~ClientLogic();

	void OnIncomingEvent();

	void Login(const TCHAR* strToken);
	void Logout();
	void AvatarCreate();
	void BackToSelect();
	void RequestCreateAvatar(const TCHAR* strAvatarName);
	void RequestSelectAvatar(const TCHAR* strAvatarName);
	void RequestSelectChannel(const TCHAR* strChannelName);
	void RequestLeaveChannel();
	void SendChatMessage(const TCHAR* strMessage);

private:
	ClientBase* m_pClientBase;
};

#endif
