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

	void SteamInit( );

	void ShutdownSteam( );

	void update( );

public:
	int8 run( );

	CApp( );
	~CApp( );
};
