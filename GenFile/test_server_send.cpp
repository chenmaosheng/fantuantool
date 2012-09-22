#include "test_server_send.h"
#include "data_stream.h"
#include "packet.h"

int32 Test_Server_Send::LoginNtf(void* pClient, uint32 iSessionId, const char* strNickName)
{
	OutputStream stream;
	stream.Serialize(iSessionId);
	uint16 iLength = (uint16)strlen(strNickName);
	stream.Serialize(iLength);
	stream.Serialize(iLength, strNickName);

	return Sender::SendPacket(pClient, 1, stream.GetDataLength(), stream.GetBuffer());
}