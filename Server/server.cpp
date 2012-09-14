#include "server.h"
#include "Command.h"
#include "connection.h"
#include "worker.h"
#include "acceptor.h"

std::vector<Connection*> Server::clients;
std::vector< std::pair<Connection*, std::string > > Server::nicknames;

Server::Server() : worker_(NULL)
{
}

Server::~Server()
{

}

void Server::Init(uint16 port)
{
	handler_.OnConnection = &OnConnection;
	handler_.OnDisconnect = &OnDisconnect;
	handler_.OnData = &OnData;
	handler_.OnConnectFailed = &OnConnectFailed;

	worker_ = new Worker;
	worker_->Init();

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	acceptor_ = new Acceptor;
	acceptor_->Init(&addr, worker_, &handler_);
}

void Server::Start()
{
	acceptor_->Start();
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
