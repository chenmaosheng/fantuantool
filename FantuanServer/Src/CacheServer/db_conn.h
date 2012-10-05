/*****************************************************************************************
	filename:	db_conn.h
	created:	10/06/2012
	author:		chen
	purpose:	db connector object

*****************************************************************************************/

#ifndef _H_DB_CONN
#define _H_DB_CONN

#include "common.h"
#include "mysql.h"

class DBConn
{
public:
	DBConn();
	
	// initialize db connector
	void Init();
	// destroy connector
	void Destroy();

	// start, stop and join the thread
	void Start();
	void Stop();
	void Join();

	MYSQL* GetConnector();

private:
	bool _InitConnector();

	static uint32 WINAPI _HandleDBConn(PVOID pParam);

private:
	MYSQL* m_pMySQL;
	HANDLE m_hThread;
	bool m_bQuit;

	char m_strDBName[256];
	char m_strIP[32];
	uint16 m_iPort;
	char m_strUserName[32];
	char m_strPassword[32];
};

#endif
