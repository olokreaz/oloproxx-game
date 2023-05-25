#pragma once

#include <iostream>

#include <steam/isteamnetworkingsockets.h>
#include <steam/isteamnetworkingsockets.h>
#include <steam/isteamnetworkingutils.h>
#include <steam/steamnetworkingtypes.h>
#include <steam/steamtypes.h>

#include <spdlog/spdlog.h>

class CServer final {
	int16            m_nPort = { 0 };
	SteamIPAddress_t m_ip;

	void update( ) { }
	void event( ) {}

	void m_init( ) {}
	void m_shutdown( ) {}
	void m_run( ) {}

public:
	void port( int16 port ) { m_nPort = port; }

	CServer( );
	~CServer( );

	void run( ) { }
};
