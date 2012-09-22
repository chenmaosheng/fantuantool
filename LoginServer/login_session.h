#ifndef _H_LOGIN_SESSION
#define _H_LOGIN_SESSION

#include "session.h"

class LoginServerLoop;
class LoginSession : public Session
{
public:
	typedef Session super;

	LoginSession();
	~LoginSession();

	void Clear();		// when reuse, clear the session

	int32 OnConnection(ConnID connId);
	void OnDisconnect();
	void Disconnect();

	void LoginReq(const char* strNickname);

public:
	static LoginServerLoop* m_pMainLoop;

private:
	TCHAR m_strAccountName[64];
};

#endif
