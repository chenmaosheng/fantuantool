#include "common.h"
#include "log.h"
#include "..\..\external\mysql\include\mysql.h"

int main()
{
	//Log* m_pLogSystem = Log::GetInstance();
	//m_pLogSystem->Init(0);

	//LogDevice* pDevice = NULL;
	//// screen log
	//pDevice = m_pLogSystem->CreateAndAddLogDevice(Log::LOG_DEVICE_CONSOLE);

	//// start log system
	//m_pLogSystem->Start();

	//m_pLogSystem->SetLogTypeString(LOG_SERVER, _T("Server"));

	//LOG_STT(LOG_SERVER, _T("Initialize log system success"));

	//for (int i = 0; i < 5; ++i)
	//{
	//	LOG_ERR(LOG_SERVER, _T("Error"));
	//	LOG_WAR(LOG_SERVER, _T("Warning"));
	//	LOG_DBG(LOG_SERVER, _T("Debug"));
	//}

	MYSQL* m_pMySQL = mysql_init(NULL);
	m_pMySQL = mysql_real_connect(m_pMySQL, "127.0.0.1", "root", "passw0rD", NULL, 3306, NULL, 0);
	int rc = mysql_select_db(m_pMySQL, "fantuan");

	system("pause");
	return 0;
}