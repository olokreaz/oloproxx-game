#include <include/App/App.h>

#include <steam/isteamnetworkingsockets.h>

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

void CApp::SteamShutdown( )
{
	spdlog::info( "Shoutdown SteamAPI" );
	SteamAPI_Shutdown( );
}

void CApp::update( )
{
	std::string b;
	while ( true ) if ( std::getline( std::cin, b ) ) if ( b == "/stop" ) return;
}

void CApp::recv( ) { ISteamNetworkingMessage *msg; }
void CApp::send( ) {}
void CApp::handler( ) {}

int8 CApp::run( )
{

	// auto [ t_recv, t_send, t_handler ] = m_taskEventLoop.emplace( [this] { this->recv( ); }
	// 							      , [this] { this->send( ); }
	// 							      , [this] { this->handler( ); }
	// 							    );
	//
	// spdlog::info( "StartEvent loop" );
	//
	// while ( s_bQuit ) m_executorEventLoop.run( m_taskEventLoop );
	//
	// m_executorEventLoop.wait_for_all( );

	return 0;
}

CApp::CApp( )
{
	tf::Taskflow taskflow;

	auto task_SteamInit = taskflow.emplace( [this] { SteamInit( ); } );

	// m_executor.run_and_wait( taskflow );
}

CApp::~CApp( )
{
	tf::Taskflow taskflow;
	auto         task_ShoutDownSteam = taskflow.emplace( [this] { this->SteamShutdown( ); } );

};
