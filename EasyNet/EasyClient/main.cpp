#include "easy_client.h"
#include "easy_client_message.h"

void SendCommand()
{
	char command[128] = {0};
	char message[128] = {0};
	scanf_s("%s", &command, _countof(command));
	if (_stricmp(command, "Ping") == 0)
	{
		ClientSend::PingReq(g_pClient, 0);
	}
	else if (_stricmp(command, "Chat") == 0)
	{
		scanf_s("%s", &message, _countof(message));
		ClientSend::ChatReq(g_pClient, strlen(message), message);
	}
}

int __cdecl _tmain(int argc, char **argv)
{
	g_pClient = new EasyClient;
	g_pClient->Init();
	g_pClient->Login(inet_addr("192.168.0.110"), 9001);

	while (true)
	{
		SendCommand();
	}

	g_pClient->Destroy();

	return 0;
}
