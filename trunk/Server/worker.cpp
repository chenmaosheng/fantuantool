#include "worker.h"
#include "connection.h"
#include "context.h"
#include "Command.h"
#include <process.h>

Worker::Worker()
{
}

Worker::~Worker()
{
}

void Worker::Init()
{
	iocp_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	HANDLE hWorkerThread = (HANDLE)_beginthreadex(NULL, 0, &Worker::WorkerThread, this, 0, NULL);
	CloseHandle(hWorkerThread);
}

void Worker::Destroy()
{
	PostQueuedCompletionStatus(iocp_, 0, 0, NULL);
	if (iocp_)
	{
		CloseHandle(iocp_);
	}
}

uint32 WINAPI Worker::WorkerThread(PVOID pParam)
{
	BOOL bResult;
	DWORD dwNumRead;
	Connection* pConnection = NULL;
	LPOVERLAPPED lpOverlapped = NULL;
	Context* pContext = NULL;

	Worker* pWorker = (Worker*)pParam;

	while (true)
	{
		bResult = GetQueuedCompletionStatus(pWorker->iocp_, &dwNumRead, (ULONG_PTR*)&pConnection, &lpOverlapped, INFINITE);
		if (lpOverlapped)
		{
			pContext = (Context*)((char*)lpOverlapped - CTXOFFSET);
			switch(pContext->operation_type_)
			{
			case OPERATION_RECV:
				{
					if (dwNumRead == 0)
					{
						printf("client %s disconnected\n", inet_ntoa(pConnection->sockAddr_.sin_addr));
						
						LogoutPkt pkt;
						pkt.connID = int(pConnection);
						pkt.len = sizeof(pkt.connID);
						
						pWorker->DeleteClient(pConnection);
						closesocket(pConnection->socket_);

						pWorker->SendToAll((char*)&pkt, pkt.len + sizeof(Header));
					}
					else
					{
						printf("client %s send something\n", inet_ntoa(pConnection->sockAddr_.sin_addr));
						Header* header = (Header*)pContext->buffer_;
						if (header->type == LOGIN)
						{
							LoginPkt* pkt = (LoginPkt*)header;
							pkt->connID = (int)pConnection;
							pWorker->nicknames.push_back( std::pair<Connection*, std::string>(pConnection, pkt->nickname) );
							for (size_t i = 0; i < pWorker->clients.size(); ++i)
							{
								if (pWorker->clients.at(i)->socket_ != pConnection->socket_)
								{
									LoginPkt newPkt;
									strcpy_s(newPkt.nickname, sizeof(newPkt.nickname), pWorker->GetNickName(pWorker->clients.at(i)).c_str());
									newPkt.len = (int)strlen(newPkt.nickname) + sizeof(newPkt.connID);
									newPkt.connID = (int)(pWorker->clients.at(i));
									pConnection->AsyncSend(newPkt.len + sizeof(Header), (char*)&newPkt);
									printf("send to %d\n", pConnection->socket_);
								}
							}

							pWorker->SendToAll((char*)pkt, header->len + sizeof(Header));
						}
						else
						{
							pWorker->SendToAll(pContext->buffer_, header->len + sizeof(Header));
						}

						pConnection->AsyncRecv();
					}
					
				}
				break;

			case OPERATION_SEND:
				{
					
				}
				break;
			default:
				break;
			}
		}

	}
}

std::string Worker::GetNickName(Connection* pConnection)
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
void Worker::DeleteClient(Connection* pConnection)
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
}

void Worker::SendToAll(char* buf, int len)
{
	for (std::vector<Connection*>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		(*it)->AsyncSend(len, buf);
	}
}
