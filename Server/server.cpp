#include "server.h"
#include "Command.h"
#include "connection.h"
#include "worker.h"
#include "acceptor.h"
#include "context_pool.h"
#include "handler.h"

std::vector<Connection*> Server::clients;
std::vector< std::pair<Connection*, std::string > > Server::nicknames;

Server::Server()
{
}

Server::~Server()
{

}

void Server::Init(uint16 port)
{
	int32 iRet = 0;
	static Handler handler;
	handler.OnConnection = &OnConnection;
	handler.OnDisconnect = &OnDisconnect;
	handler.OnData = &OnData;
	handler.OnConnectFailed = &OnConnectFailed;

	iRet = InitAcceptor(0, port, &handler);
	if (iRet != 0)
	{
		return;
	}

	//StartAcceptor();
}

void Server::Start()
{
	StartAcceptor();
}

bool CALLBACK Server::OnConnection(ConnID connID)
{
	Connection* pConnection = (Connection*)connID;
	clients.push_back(pConnection);
	return true;
}

void CALLBACK Server::OnDisconnect(ConnID connID)
{
	Connection* pConnection = (Connection*)connID;
	LogoutPkt pkt;
	pkt.connID = (int)connID;
	pkt.len = sizeof(pkt.connID);

	DeleteClient(pConnection);
	
	SendToAll((char*)&pkt, pkt.len + sizeof(Header));
}

void CALLBACK Server::OnData(ConnID connID, uint16 iLen, char* pBuf)
{
	Connection* pConnection = (Connection*)connID;
	Header* header = (Header*)pBuf;
	if (header->type == LOGIN)
	{
		LoginPkt* pkt = (LoginPkt*)header;
		pkt->connID = (int)connID;
		nicknames.push_back( std::pair<Connection*, std::string>(pConnection, pkt->nickname) );
		for (size_t i = 0; i < clients.size(); ++i)
		{
			if (clients.at(i)->socket_ != pConnection->socket_)
			{
				LoginPkt newPkt;
				strcpy_s(newPkt.nickname, sizeof(newPkt.nickname), GetNickName(clients.at(i)).c_str());
				newPkt.len = (int)strlen(newPkt.nickname) + sizeof(newPkt.connID);
				newPkt.connID = (int)(clients.at(i));
				pConnection->AsyncSend(newPkt.len + sizeof(Header), (char*)&newPkt);
				printf("send to %d\n", pConnection->socket_);
			}
		}

		SendToAll((char*)pkt, header->len + sizeof(Header));
	}
	else
	{
		SendToAll(pBuf, header->len + sizeof(Header));
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
			delete (*it);
			clients.erase(it);
			break;
		}
	}

	closesocket(pConnection->socket_);
}

void Server::SendToAll(char* buf, int len)
{
	for (std::vector<Connection*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		(*it)->AsyncSend(len, buf);
	}
}
