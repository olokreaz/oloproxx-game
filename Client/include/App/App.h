#pragma once
#include <commandline.h>
#include <map>
#include <spdlog/spdlog.h>
#include <steam/isteamutils.h>
#include <steam/steam_api.h>
#include <taskflow/taskflow.hpp>

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

enum class EModeRun {
	none = -1
	, all
	, client
	, server
	,
};

class CApp {
	void SteamInit( );
	void SteamShutdown( );

	void     update( );
	EModeRun m_eMode;

	void recv( );
	void send( );
	void handler( );

public:
	int8 run( );

	EModeRun mode( ) { return m_eMode; }
	auto     mode( EModeRun m ) { m_eMode = m; }
	

	CApp( );
	~CApp( );

	static inline bool s_bQuit = { false };
};
