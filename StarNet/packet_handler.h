#ifndef _H_PACKET_HANDLER
#define _H_PACKET_HANDLER

#include "data_stream.h"

//template<typename ClassType>
struct PacketHandlerFactory
{
	//typedef bool (ClassType::*Handler)(void*, ServerPacket&);
	typedef bool (CALLBACK *Handler)(void*, InputStream&);
	Handler* m_pHandler;
};

#define PACKET_HANDLER_FACTORY_SIZE 255

extern PacketHandlerFactory g_PacketHandlerFactories[PACKET_HANDLER_FACTORY_SIZE];



#endif
