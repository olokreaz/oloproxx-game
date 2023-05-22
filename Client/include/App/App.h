#pragma once
#include <commandline.h>
#include <map>
#include <spdlog/spdlog.h>
#include <steam/isteamutils.h>
#include <steam/steam_api.h>
#include <taskflow/taskflow.hpp>

#include "include/Socket/Client.h"
#include "include/Socket/Server.h"

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
public:
	struct IEtc {
		CServer server;
		CClient client;
	};

private:
	tf::Taskflow m_AppTask;
	IEtc         m_etc;

	void SteamInit( );
	void SteamShutdown( );
	void update( );
	void recv( );
	void send( );
	void handler( );


public:
	int8 run( );

	CApp( );
	~CApp( );

	IEtc* operator->( ) { return &m_etc; }

	static inline EModeRun m_eMode = { EModeRun::none };
	static inline bool     s_bQuit = { false };
};
