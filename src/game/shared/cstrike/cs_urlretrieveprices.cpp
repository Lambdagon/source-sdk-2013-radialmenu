//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================

//#include "stdafx.h"
#ifdef _WIN32
#include "winlite.h"
#include "winsock.h"
#elif POSIX
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define SOCKET int
#define LPSOCKADDR struct sockaddr *
#define SOCKADDR_IN struct sockaddr_in
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define closesocket close
#endif
#include "tier1/strtools.h"
#include "KeyValues.h"
#include "utlbuffer.h"
#include "tier1/checksum_crc.h"
#include "tier1/convar.h"
#include "cbase.h"
#include "cs_gamestats.h"
#include "cs_gamerules.h"
#include "cs_urlretrieveprices.h"

#if _DEBUG
#define WEEKLY_PRICE_URL "http://gamestats/weeklyprices.dat"
#else
#define WEEKLY_PRICE_URL "http://www.steampowered.com/stats/csmarket/weeklyprices.dat"
#endif
