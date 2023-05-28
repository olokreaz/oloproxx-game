#include <include/App/App.h>

#include <commandline.h>
#include <steam/isteamnetworkingsockets.h>

void CApplication::SteamInit( )
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

void CApplication::SteamShutdown( )
{
	spdlog::info( "Shoutdown SteamAPI" );
	SteamAPI_Shutdown( );
}

void CApplication::update( ) { }

void CApplication::cli_parser_show_opt( clipp::group *cl, std::string str )
{
	std::cout << clipp::make_man_page( *cl ) << std::endl;
}

bool CApplication::cli_parser_start_app( clipp::group *cli )
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

void CApplication::app_start_choose_mode( )
{
	Commandline cmd;
	cmd.write( "Choose mode on run app:(server / client)" );
	while ( true )
		if ( cmd.has_command( ) ) {
			if ( cmd.get_command( ) == "server" ) {
				this->m_eMode = EModeRun::server;
				return;
			}
			if ( cmd.get_command( ) == "client" ) {
				this->m_eMode = EModeRun::client;
				return;
			}
		}
	// next call function <3
}

CApplication::CApplication( )
{
	clipp::group cli;
	if ( cli_parser_start_app( &cli ) ) cli_parser_show_opt( &cli );
	if ( this->m_eMode == EModeRun::none );

	spdlog::info( "Start application" );
}

CApplication::~CApplication( )
{
	tf::Taskflow taskflow;
	auto         task_ShoutDownSteam = taskflow.emplace( [this] { this->SteamShutdown( ); } );
	spdlog::info( "Finish application" );
}
