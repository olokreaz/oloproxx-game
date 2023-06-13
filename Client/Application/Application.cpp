#include "Application.hpp"
#include <iostream>
#include <boost/algorithm/string/trim.hpp>

void CApplication::chooseSocketType( ) {}

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
	} else if ( m_eSocketType == ESocketType::NONE ) {
		spdlog::warn( "Socket type is NONE!" );
		chooseSocketType( );
	}

	CommandHandlerInit( );
}

result< void > CApplication::run( )
{
	*m_pbQuit = false;
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

	// main code update loop
	WHILE_QUIT runLocalInputCallback( );

	spdlog::info( "Shutting down..." );

	*m_pbQuit = true;
	co_return;
}

void CApplication::shoutdown( )
{
	m_steamlogger -> info( "Shutting down Steam API..." );
	SteamAPI_Shutdown( );
	m_steamlogger -> info( "Steam API shut down." );
}

void CApplication::runLocalInputCallback( )
{
	while ( !QueueLocalInput . empty( ) ) {
		m_mutexLocalInput . lock( );
		std::string strInput = QueueLocalInput . front( );
		QueueLocalInput . pop( );
		m_mutexLocalInput . unlock( );

		if ( strInput == "quit" ) {
			*m_pbQuit = true;
			break;
		} else if ( strInput == "help" ) {
			spdlog::info( "Available commands:" );
			spdlog::info( "quit - quit the application" );
			spdlog::info( "help - show this help" );
		} else spdlog::warn( "Unknown command: {}", strInput );
	}
}

CApplication::operator bool( ) const { return m_pbQuit; }
CApplication::CApplication( bool *b_quit ): m_pbQuit { b_quit } {}
