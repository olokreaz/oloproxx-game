#include "Application.hpp"
#include <iostream>
#include <boost/algorithm/string/trim.hpp>

void CApplication::SteamInit( )
{
	m_steamlogger -> info( "Initializing Steam API..." );

	m_steamlogger -> info( "Checking on exist steam_appid.txt file.." );

	if ( !SteamAPI_Init( ) ) {
		m_steamlogger -> error( "SteamAPI_Init() failed!" );
		*m_pbQuit = true;
		return;
	}

	m_steamlogger -> info( "Steam API initialized." );
}

void CApplication::init( ESocketType eSocketType )
{
	m_eSocketType = eSocketType;

	SteamInit( );

	if ( m_eSocketType == ESocketType::CLIENT ) {
		m_socket . m_client = std::make_shared< CClient >( m_pbQuit );
		m_socket . m_client -> init( );
	} else if ( m_eSocketType == ESocketType::SERVER ) {
		m_socket . m_server = std::make_shared< CServer >( m_pbQuit );
		m_socket . m_server -> init( );
	} else if ( m_eSocketType == ESocketType::ALL ) {
		m_socket . m_server = std::make_shared< CServer >( m_pbQuit );
		m_socket . m_client = std::make_shared< CClient >( m_pbQuit );

		m_socket . m_server -> init( );
		m_socket . m_client -> init( );
	}
}

result< void > CApplication::run( )
{
	if ( m_eSocketType == ESocketType::CLIENT ) m_socket . m_client -> run( );
	else if ( m_eSocketType == ESocketType::SERVER ) m_socket . m_server -> run( );
	else if ( m_eSocketType == ESocketType::ALL ) {
		m_socket . m_server -> run( );
		m_socket . m_client -> run( );
	}

	m_pThreadLocalInput = new std::thread( [ & ]( )-> result< void >
							{
								WHILE {
									std::string strInput;
									std::getline( std::cin, strInput );
									boost::trim( strInput );
									if ( strInput . empty( ) ) continue;
									m_mutexLocalInput . lock( );
									QueueLocalInput . push( strInput );
									m_mutexLocalInput . unlock( );
								}
							}
						);
	m_pThreadLocalInput -> detach( );

	co_return;
}

void CApplication::shoutdown( )
{
	m_steamlogger -> info( "Shutting down Steam API..." );
	SteamAPI_Shutdown( );
	m_steamlogger -> info( "Steam API shut down." );
}

CApplication::operator bool( ) const { return m_pbQuit; }
CApplication::CApplication( bool *b_quit ): m_pbQuit { b_quit } {}
