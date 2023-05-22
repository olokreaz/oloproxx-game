#pragma once
#include <steam/isteamnetworkingsockets.h>
#include <steam/isteamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingtypes.h>
#include <steam/steamtypes.h>

class CServer final {
	int16            m_nPort = { 0 };
	SteamIPAddress_t m_ip;

public:
	int16& port( ) { return m_nPort; }
	CServer( );
	~CServer( );

	void run( );
};
