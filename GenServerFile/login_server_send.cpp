#include "login_server_send.h"
#include "data_stream.h"
#include "packet.h"

int32 LoginServerSend::LoginNtf(void* pClient, uint32 iGateIP, uint16 iGatePort)
{
	OutputStream stream;
	stream.Serialize(iGateIP);
	stream.Serialize(iGatePort);

	return Sender::SendPacket(pClient, (SERVER_FILTER_LOGIN>>8) | 0, stream.GetDataLength(), stream.GetBuffer());
}