#include "server.h"
#include "Command.h"
#include "connection.h"
#include "worker.h"
#include "acceptor.h"
#include "context_pool.h"
#include "handler.h"
#include "log.h"
#include "log_device_console.h"

std::vector<Connection*> Server::clients;
std::vector< std::pair<Connection*, std::string > > Server::nicknames;

Server::Server()
{
}

Server::~Server()
{

}

int32 Server::Init()
{
	int32 iRet = 0;
	static Handler handler;
	handler.OnConnection = &OnConnection;
	handler.OnDisconnect = &OnDisconnect;
	handler.OnData = &OnData;
	handler.OnConnectFailed = &OnConnectFailed;

	LogDevice* pDevice = new LogDeviceConsole(NULL);
	Log::Instance().AddLogDevice(pDevice);
	Log::Instance().Start();

	iRet = ServerBase::Init();
	if (iRet != 0)
	{
		return -1;
	}

	iRet = InitAcceptor(0, 5150, &handler, 2);
	if (iRet != 0)
	{
		return -2;
	}

	StartAcceptor();

	TCHAR dummy[] = TEXT("Hello, World");
	Log::Instance().Push(dummy, wcslen(dummy));
	return 0;
}

void Server::Destroy()
{
	ServerBase::Destroy();
}

void Server::Start()
{
}

bool CALLBACK Server::OnConnection(ConnID connId)
{
	Connection* pConnection = (Connection*)connId;
	clients.push_back(pConnection);
	return true;
}

void CALLBACK Server::OnDisconnect(ConnID connId)
{
	Connection* pConnection = (Connection*)connId;
	Server* pServer = (Server*)pConnection->acceptor_->GetServer();
	DeleteClient(pConnection);
	
	for (std::vector<Connection*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		LogoutPkt* pkt = (LogoutPkt*)pServer->context_pool_->PopOutputBuffer();
		pkt->type = LOGOUT;
		pkt->connId = (int)connId;
		pkt->len = sizeof(pkt->connId);
		(*it)->AsyncSend(pkt->len + sizeof(Header), (char*)pkt);
	}
}

void CALLBACK Server::OnData(ConnID connId, uint16 iLen, char* pBuf)
{
	Connection* pConnection = (Connection*)connId;
	Header* header = (Header*)pBuf;
	Server* pServer = (Server*)pConnection->acceptor_->GetServer();
	if (header->type == LOGIN)
	{
		LoginPkt* pkt = (LoginPkt*)header;
		pkt->connId = (int)connId;
		nicknames.push_back( std::pair<Connection*, std::string>(pConnection, pkt->nickname) );
		for (size_t i = 0; i < clients.size(); ++i)
		{
			if (clients.at(i)->socket_ != pConnection->socket_)
			{
				LoginPkt* newPkt = (LoginPkt*)pServer->context_pool_->PopOutputBuffer();
				newPkt->type = LOGIN;
				strcpy_s(newPkt->nickname, sizeof(newPkt->nickname), GetNickName(clients.at(i)).c_str());
				newPkt->len = (int)strlen(newPkt->nickname) + sizeof(newPkt->connId);
				newPkt->connId = (int)(clients.at(i));
				pConnection->AsyncSend(newPkt->len + sizeof(Header), (char*)newPkt);
				printf("send to %d\n", pConnection->socket_);
			}
		}

		for (std::vector<Connection*>::iterator it = clients.begin(); it != clients.end(); ++it)
		{
			char* newBuf = pServer->context_pool_->PopOutputBuffer();
			memcpy(newBuf, pBuf, iLen);
			(*it)->AsyncSend(iLen, newBuf);
		}
	}
	else
	{
		for (std::vector<Connection*>::iterator it = clients.begin(); it != clients.end(); ++it)
		{
			char* newBuf = pServer->context_pool_->PopOutputBuffer();
			memcpy(newBuf, pBuf, iLen);
			(*it)->AsyncSend(iLen, newBuf);
		}
	}
}

void CALLBACK Server::OnConnectFailed(void*)
{

}

std::string Server::GetNickName(Connection* pConnection)
{
	std::vector< std::pair<Connection*, std::string > >::iterator it = nicknames.begin();
	while (it != nicknames.end())
	{
		if ((*it).first == pConnection)
		{
			return (*it).second;
		}

		++it;
	}

	return "";
}
void Server::DeleteClient(Connection* pConnection)
{
	for (std::vector< std::pair<Connection*, std::string > >::iterator it2 = nicknames.begin(); it2 != nicknames.end(); ++it2)
	{
		if ((*it2).first == pConnection)
		{
			nicknames.erase(it2);
			break;
		}
	}

	for (std::vector<Connection*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if ((*it) == pConnection)
		{
			Connection::Close(*it);
			clients.erase(it);
			break;
		}
	}
}

