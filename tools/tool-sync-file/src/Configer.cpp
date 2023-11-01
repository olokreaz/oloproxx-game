#include <app/Configer.hpp>

#include <filesystem>
#include <fstream>
#include <vector>

#include <libconfig.h++>

#include <fmt/std.h>
#include <spdlog/spdlog.h>

import app.commiting.config;

void help::CConfig::load( fs::path path )
{
	this -> m_config_path = config::kAppdata / path;

	try
	{
		m_config . readFile( this -> m_config_path . string( ) );

		const lc::Setting &root = m_config . getRoot( );

		{
			std::string src  = root[ "global" ][ "source" ];
			std::string dest = root[ "global" ][ "destination" ];

			this -> source      = src;
			this -> destination = dest;
		}

		this -> special . reserve( root[ "paths" ] . getLength( ) );
		for ( int i = 0; i < root[ "paths" ] . getLength( ); ++i )
		{
			const auto &pair = root[ "paths" ][ i ];
			CSpecial    obj;
			obj . pattern     =  std::string ( pair[ "pattern" ]);
			obj . destination = std::string  (pair[ "destination" ]);
			obj . bRelease    = pair . lookup( "release" ) ? ( bool ) pair[ "release" ] : std::nullopt;
			this -> special . insert( { pair[ "pattern" ], obj } );
		}

		this -> ignore . reserve( root[ "ignore" ] . getLength( ) );
		for ( int i = 0; i < root[ "ignore" ] . getLength( ); ++i ) this -> ignore . push_back( root[ "ignore" ][ i ] );
	} catch ( const lc::ParseException &e )
	{
		spdlog::error( "Error in config file: {}, {}, {}", e . what( ), e . getFile( ), e . getLine( ) );
		abort( );
	}
	catch ( const lc::FileIOException &e )
	{
		spdlog::error( "Error in {}: {}", this -> m_config_path, e . what( ) );
		abort( );
	}
	catch ( const std::exception &e )
	{
		spdlog::error( "Error in {}: {}", this -> m_config_path, e . what( ) );
		abort( );
	}
	m_hash = hash_value( *this );
}

void help::CConfig::save( fs::path path )
{
	lc::Config cfg;
	auto &     root                   = cfg . getRoot( );
	root[ "global" ][ "source" ]      = this -> source . string( );
	root[ "global" ][ "destination" ] = this -> destination . string( );

	for ( auto &[ _, obj ] : this -> special )
	{
		auto &pair                       = root[ "paths" ] . add( lc::Setting::TypeGroup );
		auto &[ pattern, dest, release ] = obj;
		pair[ "pattern" ]                = pattern;
		pair[ "destination" ]            = dest;
		if ( release ) pair[ "release" ] = *release;
	}

	for ( auto &i : this -> ignore ) root[ "ignore" ] . add( lc::Setting::TypeString ) = i;

	cfg . writeFile( path . string( ) );
}
