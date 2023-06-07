#include "Application.hpp"

void CApplication::SteamInit( )
{
	m_steamlogger->info( "Initializing Steam API..." );

	m_steamlogger->info( "Checking on exist steam_appid.txt file.." );

	if ( !SteamAPI_Init( ) ) {
		m_steamlogger->error( "SteamAPI_Init() failed!" );
		*m_bQuit = true;
		return;
	}

	m_steamlogger->info( "Steam API initialized." );
}

void CApplication::init( ESocketType eSocketType )
{
	m_eSocketType = eSocketType;

	SteamInit( );

	if ( m_eSocketType == ESocketType::CLIENT ) {
		m_socket.m_client = std::make_shared< CClient >( m_bQuit );
		m_socket.m_client->init( );
	} else if ( m_eSocketType == ESocketType::SERVER ) {
		m_socket.m_server = std::make_shared< CServer >( m_bQuit );
		m_socket.m_server->init( );
	} else if ( m_eSocketType == ESocketType::ALL ) {
		m_socket.m_server = std::make_shared< CServer >( m_bQuit );
		m_socket.m_server->init( );

		m_socket.m_client = std::make_shared< CClient >( m_bQuit );
		m_socket.m_client->init( );
	}
}

concurrencpp::result< void > CApplication::run( ) { co_return; }

void CApplication::shoutdown( )
{
	m_steamlogger->info( "Shutting down Steam API..." );
	SteamAPI_Shutdown( );
	m_steamlogger->info( "Steam API shut down." );
}

CApplication::operator bool( ) const { return m_bQuit; }
CApplication::CApplication( bool *b_quit ): m_bQuit { b_quit } {}
