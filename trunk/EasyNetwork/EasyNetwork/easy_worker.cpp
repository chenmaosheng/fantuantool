#include "easy_worker.h"
#include "easy_acceptor.h"
#include <process.h>
#include <vector>
#include "easy_connection.h"

bool ReadData(EasyConnection* conn)
{
	int nBytes = recv(conn->socket_, conn->buffer_ + conn->buffer_size_, 1024 - conn->buffer_size_, 0);
	if (nBytes == 0)
	{
		printf("socket was closed\n");
		return false;
	}

	conn->buffer_size_ += nBytes;

	return true;
}

bool WriteData(EasyConnection* conn)
{
	int nBytes = send(conn->socket_, conn->buffer_, conn->buffer_size_, 0);
	if (nBytes == conn->buffer_size_)
	{
		conn->buffer_size_ = 0;
	}
	else
	{
		conn->buffer_size_ -= nBytes;
		memmove(conn->buffer_, conn->buffer_ + nBytes, conn->buffer_size_);
	}

	return true;
}

uint32 WINAPI EasyThread(PVOID pParam)
{
	std::vector<EasyConnection*> ConnectionList;
	EasyAcceptor* pAcceptor = (EasyAcceptor*)pParam;
	while (true)
	{
		fd_set readFDs, writeFDs, exceptFDs;
		FD_ZERO(&readFDs);
		FD_ZERO(&writeFDs);
		FD_ZERO(&exceptFDs);
		FD_SET(pAcceptor->socket_, &readFDs);
		FD_SET(pAcceptor->socket_, &writeFDs);
		std::vector<EasyConnection*>::iterator it = ConnectionList.begin();
		while (it != ConnectionList.end())
		{
			if ((*it)->buffer_size_ < 1024)
			{
				FD_SET((*it)->socket_, &readFDs);
			}
			if ((*it)->buffer_size_ > 0)
			{
				FD_SET((*it)->socket_, &writeFDs);
			}
			FD_SET((*it)->socket_, &exceptFDs);

			++it;
		}


		timeval val;
		val.tv_sec = 0;
		val.tv_usec = 10000;
		if (select(0, &readFDs, &writeFDs, &exceptFDs, &val) > 0)
		{
			if (FD_ISSET(pAcceptor->socket_, &readFDs))
			{
				EasyConnection* pConnection = AcceptConnection(pAcceptor);
				ConnectionList.push_back(pConnection);
				
			}
			else if (FD_ISSET(pAcceptor->socket_, &exceptFDs))
			{
				return -1;
			}

			std::vector<EasyConnection*>::iterator it = ConnectionList.begin();
			while (it != ConnectionList.end())
			{
				bool bOK = true;
				if (FD_ISSET((*it)->socket_, &exceptFDs))
				{
					bOK = false;
				}
				else
				{
					if (FD_ISSET((*it)->socket_, &readFDs))
					{
						bOK = ReadData(*it);
						FD_CLR((*it)->socket_, &readFDs);
						(*it)->buffer_[0] = '\0';
						(*it)->buffer_size_ = 0;
					}
					if (FD_ISSET((*it)->socket_, &writeFDs))
					{
						bOK = WriteData(*it);
						FD_CLR((*it)->socket_, &writeFDs);
					}
				}

				if (!bOK)
				{
					_ASSERT(false);
				}
				else
				{
					++it;
				}
			}
		}
	}
}


void StartWorker(EasyAcceptor* pAcceptor)
{
	_beginthreadex(NULL, 0, &EasyThread, pAcceptor, 0, NULL);
}
