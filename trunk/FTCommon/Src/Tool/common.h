/*****************************************************************************************
	filename:	common.h
	created:	09/27/2012
	author:		chen
	purpose:	common headers for all other files

*****************************************************************************************/

#ifndef _H_COMMON
#define _H_COMMON

#include <winsock2.h>
#include <mswsock.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include "type.h"
#include "util.h"

// common defines
#define ACCOUNTNAME_MAX		32
#define PASSWORD_MAX		32
#define FSM_STATE_MAX		256
#define MAX_TOKEN_LEN		512
#define AVATARNAME_MAX		32
#define AVATARCOUNT_MAX		10

// todo: temp data
const static char g_LoggedInNtf[] = "LoggedIn";

enum
{
	// all C/S filters defined
	CLIENT_FILTER_LOGIN,
	SERVER_FILTER_LOGIN,
	CLIENT_FILTER_GATE,
	SERVER_FILTER_GATE,
};

#endif
