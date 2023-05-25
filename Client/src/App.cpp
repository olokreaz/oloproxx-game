#include <include/App/App.h>

#include <commandline.h>
#include <steam/isteamnetworkingsockets.h>

void CApp::SteamInit( )
{
	spdlog::info( "Start SteamAPI init" );
	if ( !SteamAPI_Init( ) ) spdlog::error( "Failed SteamAPI init" );
	else spdlog::info( "Finish SteamAPI init" );

	SteamNetworkingUtils( )->SetDebugOutputFunction( k_ESteamNetworkingSocketsDebugOutputType_Msg
							 , [] (
						 ESteamNetworkingSocketsDebugOutputType nType
						 , char const *                         pszMsg
					 )
								 {
									 switch ( nType ) {
										 case
										 k_ESteamNetworkingSocketsDebugOutputType_Bug
										 : spdlog::error( "Steam NetWork: {}"
												  , pszMsg
												);
											 break;
										 case
										 k_ESteamNetworkingSocketsDebugOutputType_Error
										 : spdlog::critical( "Steam NetWork: {}"
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

	SteamUtils( )->SetWarningMessageHook( [] ( int lvl, char const *msg )
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

void CApp::update( ) { }

void CApp::cli_parser_show_opt( clipp::group *cl, std::string str )
{
	std::cout << clipp::make_man_page( *cl ) << std::endl;
}

bool CApp::cli_parser_start_app( clipp::group *cli )
{
	int         port;
	std::string address;
	auto        cliS = (
		clipp::command( "server" ) & clipp::opt_value( "listen port", port )
	);

	auto cliC = (
		clipp::command( "client" ) & ( clipp::opt_value( "address:port", address ) |
					       clipp::opt_value( "address" )
					       & clipp::value( "port", port ) )
	);

	auto cliHelp = (
		clipp::command( "help", "?" )
		& clipp::opt_value( "command" ).call( [&] ( std::string arg )
							     {
								     if (
									     arg
									     ==
									     "client" )
									     cli_parser_show_opt( &
												 cliC
												);
								     else if
								     (
									     arg
									     ==
									     "server" )
									     cli_parser_show_opt( &
												 cliS
												);
								     else
									     cli_parser_show_opt( &
												 *cli
												);
								     exit( 0 );
							     }
						    )
	);

	auto log_lvl = (

		clipp::option( "--log", "-l" ) & clipp::value( "level" ).
						 call( [] ( std::string arg )
							      {
								      if ( arg == "trace" )
									      spdlog::set_level( spdlog::level::trace
											       );
								      else if ( arg == "debug" )
									      spdlog::set_level( spdlog::level::debug
											       );
								      else if ( arg == "info" )
									      spdlog::set_level( spdlog::level::info
											       );
								      else if ( arg == "warn" )
									      spdlog::set_level( spdlog::level::warn
											       );
								      else if ( arg == "err" )
									      spdlog::set_level( spdlog::level::err
											       );
								      else if ( arg == "critical" )
									      spdlog::set_level( spdlog::level::critical
											       );
								      else if ( arg == "off" )
									      spdlog::set_level( spdlog::level::off
											       );
							      }
						     )
						 .doc( "trace, debug, info, warn, err, critical, off, default info"
						     )
	);

	*cli = ( cliHelp | cliC | cliS | log_lvl );

	return !parse( __argc, __argv, *cli ); // return true else error
}

CApp::CApp( )
{
	clipp::group cli;
	if ( cli_parser_start_app( &cli ) ) cli_parser_show_opt( &cli );
	if ( this->m_eMode == EModeRun::none ) return;

	spdlog::info( "Start application" );
}

CApp::~CApp( )
{
	tf::Taskflow taskflow;
	auto         task_ShoutDownSteam = taskflow.emplace( [this] { this->SteamShutdown( ); } );
	spdlog::info( "Finish application" );
}
