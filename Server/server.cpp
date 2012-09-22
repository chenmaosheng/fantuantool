#include "server.h"
#include "Command.h"
#include "connection.h"
#include "worker.h"
#include "acceptor.h"
#include "context_pool.h"
#include "handler.h"
#include "server_loop.h"
#include "logic_command.h"
#include "session.h"

#include "test_packet_dispatch.h"

std::vector<Connection*> Server::clients;
std::vector< std::pair<Connection*, std::string > > Server::nicknames;

Server* g_pServer = NULL;

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

	g_pServer = this;

	iRet = ServerBase::Init();
	if (iRet != 0)
	{
		return -1;
	}

	iRet = InitAcceptor(0, 5151, &handler, 2);
	if (iRet != 0)
	{
		return -2;
	}

	StartAcceptor();

	StartMainLoop();

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
	//Connection* pConnection = (Connection*)connId;
	//clients.push_back(pConnection);
	Server* pServer = (Server*)((Connection*)connId)->acceptor_->server_;

	LogicCommandOnConnect* pCommand = NULL;

	if (!pServer->m_pMainLoop)
	{
		return false;
	}

	pCommand = new LogicCommandOnConnect;
	pCommand->m_ConnId = connId;
	pServer->m_pMainLoop->PushCommand(pCommand);
	return true;
}

void CALLBACK Server::OnDisconnect(ConnID connId)
{
	/*Connection* pConnection = (Connection*)connId;
	Server* pServer = (Server*)pConnection->acceptor_->GetServer();
	DeleteClient(pConnection);
	
	for (std::vector<Connection*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		LogoutPkt* pkt = (LogoutPkt*)pServer->m_pContextPool->PopOutputBuffer();
		pkt->type = LOGOUT;
		pkt->connId = (int)connId;
		pkt->len = sizeof(pkt->connId);
		(*it)->AsyncSend(pkt->len + sizeof(Header), (char*)pkt);
	}*/
	Server* pServer = (Server*)((Connection*)connId)->acceptor_->server_;

	LogicCommandOnDisconnect* pCommand = NULL;

	if (!pServer->m_pMainLoop)
	{
		return;
	}

	pCommand = new LogicCommandOnDisconnect;
	pCommand->m_ConnId = connId;
	pServer->m_pMainLoop->PushCommand(pCommand);
}

void CALLBACK Server::OnData(ConnID connId, uint16 iLen, char* pBuf)
{
	/*Connection* pConnection = (Connection*)connId;
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
				LoginPkt* newPkt = (LoginPkt*)pServer->m_pContextPool->PopOutputBuffer();
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
			char* newBuf = pServer->m_pContextPool->PopOutputBuffer();
			memcpy(newBuf, pBuf, iLen);
			(*it)->AsyncSend(iLen, newBuf);
		}
	}
	else
	{
		for (std::vector<Connection*>::iterator it = clients.begin(); it != clients.end(); ++it)
		{
			char* newBuf = pServer->m_pContextPool->PopOutputBuffer();
			memcpy(newBuf, pBuf, iLen);
			(*it)->AsyncSend(iLen, newBuf);
		}
	}*/

	Server* pServer = (Server*)((Connection*)connId)->acceptor_->server_;

	LogicCommandOnData* pCommand = NULL;

	if (!pServer->m_pMainLoop)
	{
		return;
	}

	pCommand = new LogicCommandOnData;
	pCommand->m_ConnId = connId;
	if (!pCommand->CopyData(iLen, pBuf))
	{
		SAFE_DELETE(pCommand);
		return;
	}

	pServer->m_pMainLoop->PushCommand(pCommand);
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

int32 Server::InitMainLoop()
{
	m_pMainLoop = new ServerLoop;
	return m_pMainLoop->Init();
}

void Server::DestroyMainLoop()
{
	if (m_pMainLoop)
	{
		m_pMainLoop->Destroy();
		SAFE_DELETE(m_pMainLoop);
	}
}

void Server::InitPacketDispatch()
{
	static Test_PacketDispatch _Test_PacketDispatch;
}