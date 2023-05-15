#include <include/App/App.h>

void CApp::SteamInit( )
{
	spdlog::info( "Start SteamAPI init" );
	if ( !SteamAPI_Init( ) ) spdlog::error( "Failed SteamAPI init" );
	else spdlog::info( "Finish SteamAPI init" );

	SteamNetworkingUtils( )->SetDebugOutputFunction( k_ESteamNetworkingSocketsDebugOutputType_Msg
							 , [] (
						 ESteamNetworkingSocketsDebugOutputType nType
						 , const char *                         pszMsg
					 )
								 {
									 switch ( nType ) {
										 case
										 k_ESteamNetworkingSocketsDebugOutputType_Bug
										 : spdlog::critical( "Steam NetWork: {}"
												     , pszMsg
												   );
											 break;
										 case
										 k_ESteamNetworkingSocketsDebugOutputType_Error
										 : spdlog::error( "Steam NetWork: {}"
												  , pszMsg
												);
											 break;
										 case
										 k_ESteamNetworkingSocketsDebugOutputType_Important
										 :
										 case
										 k_ESteamNetworkingSocketsDebugOutputType_Msg
										 :
										 case
										 k_ESteamNetworkingSocketsDebugOutputType_Verbose
										 : spdlog::info( "Steam NetWork: {}"
												 , pszMsg
											       );
											 break;
										 case
										 k_ESteamNetworkingSocketsDebugOutputType_Warning
										 : spdlog::warn( "Steam NetWork: {}"
												 , pszMsg
											       );
											 break;
										 default
										 : spdlog::debug( "Steam NetWork: {}"
												  , pszMsg
												);
									 }
								 }
						       );

	SteamUtils( )->SetWarningMessageHook( [] ( auto lvl, auto msg )
						     {
							     if ( !lvl )
								     spdlog::info( "Steam: {}"
										   , msg
										 );
							     else spdlog::warn( "Steam: {}", msg );
						     }
					    );
}

void CApp::ShutdownSteam( )
{
	spdlog::info( "Shoutdown SteamAPI" );
	SteamAPI_Shutdown( );
}

void CApp::update( )
{
	std::string b;
	while ( true ) if ( std::getline( std::cin, b ) ) if ( b == "/stop" ) return;
}

int8 CApp::run( )
{
	spdlog::info( "Start App" );
	return 0;
}

CApp::CApp( )
{
	SteamInit( );

	
	
}

CApp::~CApp( ) = default;
