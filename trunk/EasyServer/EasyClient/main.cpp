#include "easy_client.h"
#include "easy_client_message.h"

void SendCommand()
{
	char command[128] = {0};
	scanf_s("%s", &command, _countof(command));
	if (_stricmp(command, "Ping") == 0)
	{
		ClientSend::PingReq(g_pClient, 0);
	}
}

int __cdecl _tmain(int argc, TCHAR **argv)
{
        g_pClient = new EasyClient;
        g_pClient->Init();
		g_pClient->Login(inet_addr("127.0.0.1"), 9001);

        while (true)
        {
                SendCommand();
        }

        g_pClient->Destroy();

        return 0;
}
