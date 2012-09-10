#ifndef _H_PACKET
#define _H_PACKET

struct RAW_PACKET
{
	unsigned short	len;
	unsigned char	buf[1];
};

struct PACKET
{
	unsigned short	len;
	unsigned short	code;
	unsigned char	buf[1];
};

#endif
