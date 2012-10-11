#include "stdafx.h"
#include "Client.h"
#include "ClientLogic.h"
#include "client_event.h"
#include "client_config.h"
#include "LoginDlg.h"

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

void ClientLogic::OnIncomingEvent()
{
	ClientEvent* pEvent = m_pClientBase->PopClientEvent();
	if (pEvent)
	{
		switch(pEvent->m_iEventId)
		{
		case EVENT_AVATAR_LIST:
			theApp.TriggerPageEvent(LOGIN_SUCCESS_EVENT);
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