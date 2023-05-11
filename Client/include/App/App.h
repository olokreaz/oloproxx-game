#pragma once
#include <commandline.h>
#include <map>
#include <spdlog/spdlog.h>
#include <steam/isteamnetworkingutils.h>
#include <steam/isteamutils.h>
#include <steam/steam_api.h>
#include <taskflow/taskflow.hpp>

extern Commandline g_cmd;
/*
 class CThread {
 	static inline std::map< int, std::thread > m_threads;

 public:
 	std::map< int, std::thread >::iterator begin( ) { return m_threads.begin( ); }
 	std::map< int, std::thread >::iterator end( ) { return m_threads.end( ); }

 	void add_thread( std::thread &&thread )
 	{
 		m_threads[ m_threads.size( ) + 1 ] = move( thread );
 		thread.detach( );
 	}

 	void join_all( )
 	{
 		for ( auto &[ id, th ] : m_threads ) th.join( );
 		m_threads.clear( );
 	}
 };
*/

// extern CThread g_ThreadPool;

template< class T > class Command {
	std::string type_name( ) const
	{
		return std::string( typeid( T ).name( ) ).substr( std::ranges::find( typeid( T ).name( ), " " ) );
	}

protected:
	const std::string m_CommaneName;

	Command( const std::string &commane_name = type_name( ) ) : m_CommaneName { commane_name }
	{
		spdlog::debug( "commnad_name: {}", m_CommaneName );
	}
};

class CApp {
	tf::Executor m_executor;
	tf::Taskflow m_taskflow;

	void SteamInit( )
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
										 /*								 		* {
	k_ESteamNetworkingSocketsDebugOutputType_None = 0,
	k_ESteamNetworkingSocketsDebugOutputType_Bug = 1, // You used the API incorrectly, or an internal error happened
	k_ESteamNetworkingSocketsDebugOutputType_Error = 2, // Run-time error condition that isn't the result of a bug.  (E.g. we are offline, cannot bind a port, etc)
	k_ESteamNetworkingSocketsDebugOutputType_Important = 3, // Nothing is wrong, but this is an important notification
	k_ESteamNetworkingSocketsDebugOutputType_Warning = 4,
	k_ESteamNetworkingSocketsDebugOutputType_Msg = 5, // Recommended amount
	k_ESteamNetworkingSocketsDebugOutputType_Verbose = 6, // Quite a bit
	k_ESteamNetworkingSocketsDebugOutputType_Debug = 7, // Practically everything
	k_ESteamNetworkingSocketsDebugOutputType_Everything = 8, // Wall of text, detailed packet contents breakdown, etc
	k_ESteamNetworkingSocketsDebugOutputType__Force32Bit = 0x7fffffff
};
										  */

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

	void ShutdownSteam( )
	{
		spdlog::info( "Shoutdown SteamAPI" );
		SteamAPI_Shutdown( );
	}

	void update( )
	{
		while ( true ) if ( g_cmd.has_command( ) ) if ( g_cmd.get_command( ) == "/stop" ) return;
	}

public:
	int8 run( )
	{
		auto [
			UpdateSocket,
			UpdateEvent
		] = m_taskflow.emplace( update, "Update" );

		return 0;
	}

	CApp( ) { SteamInit( ); }
	~CApp( ) = default;
};
