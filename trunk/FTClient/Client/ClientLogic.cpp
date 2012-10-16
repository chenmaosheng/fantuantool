#include "stdafx.h"
#include "Client.h"
#include "ClientLogic.h"
#include "client_event.h"
#include "client_config.h"
#include "LoginDlg.h"
#include "SelectDlg.h"
#include "ChannelDlg.h"

ClientLogic::ClientLogic(ClientBase* pClientBase) : m_pClientBase(pClientBase)
{
}

ClientLogic::~ClientLogic()
{

}

void ClientLogic::Login(const TCHAR* strToken)
{
	char strUTF8[MAX_TOKEN_LEN] = {0};
	WChar2Char(strToken, strUTF8, MAX_TOKEN_LEN);

	m_pClientBase->Login(inet_addr(g_pClientConfig->GetLoginHost()), g_pClientConfig->GetLoginPort(), strUTF8);
}

void ClientLogic::Logout()
{
	m_pClientBase->Logout();
}

void ClientLogic::AvatarCreate()
{
	theApp.TriggerPageEvent(CREATE_REQUEST_EVENT);
}

void ClientLogic::BackToSelect()
{
	theApp.TriggerPageEvent(BACK_EVENT);
}

void ClientLogic::RequestCreateAvatar(const TCHAR* strAvatarName)
{
	m_pClientBase->RequestCreateAvatar(strAvatarName);
}

void ClientLogic::RequestSelectAvatar(const TCHAR* strAvatarName)
{
	m_pClientBase->RequestSelectAvatar(strAvatarName);
}

void ClientLogic::RequestSelectChannel(const TCHAR* strChannelName)
{
	m_pClientBase->RequestSelectChannel(strChannelName);
}

void ClientLogic::RequestLeaveChannel()
{
	m_pClientBase->RequestLeaveChannel();
}

void ClientLogic::OnIncomingEvent()
{
	ClientEvent* pEvent = m_pClientBase->PopClientEvent();
	if (pEvent)
	{
		switch(pEvent->m_iEventId)
		{
		case EVENT_AVATAR_LIST:
			{
				ClientEventAvatarList* pEventAvatarList = (ClientEventAvatarList*)pEvent;
				theApp.TriggerPageEvent(LOGIN_SUCCESS_EVENT);
				theApp.m_pSelectDlg->ReceiveAvatarList(pEventAvatarList->m_iReturn, pEventAvatarList->m_iAvatarCount, pEventAvatarList->m_Avatar);
			}
			
			break;

		case EVENT_AVATAR_CREATE:
			{
				ClientEventAvatarCreate* pEventAvatarCreate = (ClientEventAvatarCreate*)pEvent;
				theApp.TriggerPageEvent(CREATE_SUCCESS_EVENT);
				theApp.m_pSelectDlg->ReceiveAvatarCreate(pEventAvatarCreate->m_iReturn, pEventAvatarCreate->m_Avatar);
			}
			break;

		case EVENT_AVATAR_SELECT:
			{
			}
			break;

		case EVENT_CHANNEL_LIST:
			{
				ClientEventChannelList* pEventChannelList = (ClientEventChannelList*)pEvent;
				theApp.TriggerPageEvent(SELECT_REQUEST_EVENT);
				theApp.m_pChannelDlg->ReceiveChannelList(pEventChannelList->m_iChannelCount, pEventChannelList->m_arrayChannelData);
			};
			break;

		case EVENT_CHANNEL_SELECT:
			{
				ClientEventChannelSelect* pEventChannelSelect = (ClientEventChannelSelect*)pEvent;
				theApp.TriggerPageEvent(CHANNEL_REQUEST_EVENT);
			}
			break;

		case EVENT_AVATAR_LOGOUT:
			theApp.TriggerPageEvent(LOGOUT_SUCCESS_EVENT);
			break;

		default:
			break;
		}

		FT_DELETE(pEvent);
	}
}