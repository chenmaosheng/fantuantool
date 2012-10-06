/*****************************************************************************************
	filename:	db_conn.h
	created:	10/06/2012
	author:		chen
	purpose:	db connector object

*****************************************************************************************/

#ifndef _H_DB_CONN
#define _H_DB_CONN

#include "server_common.h"
#include "mysql.h"

class DBConnPool;
class DBConn
{
public:
	DBConn();
	
	// initialize db connector
	void Init(DBConnPool* pDBConnPool);
	// destroy connector
	void Destroy();

	// start, stop and join the thread
	void Start();
	void Stop();
	void Join();

	MYSQL* GetConnector();
	void DeleteConnector();

private:
	bool _InitConnector();

	static uint32 WINAPI _HandleDBConn(PVOID pParam);

private:
	MYSQL* m_pMySQL;
	HANDLE m_hThread;
	bool m_bQuit;

	DBConnPool* m_pDBConnPool;

public:
	MYSQL_RES* m_pResult;
};

#endif
