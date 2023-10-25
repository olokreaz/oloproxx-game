#include <cmt/Configer.hpp>

#include <filesystem>
#include <fstream>
#include <vector>

#include <libconfig.h++>
#include <spdlog/spdlog.h>

namespace fs = std::filesystem;
namespace lc = libconfig;

void help::CConfig::load( fs::path path )
{
	this -> m_config_path = path;

	try
	{
		m_config . readFile( path . string( ) );

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
			this -> special . insert( { pair[ "pattern" ], pair[ "destination" ] } );
		}

		this -> ignore . reserve( root[ "ignore" ] . getLength( ) );
		for ( int i = 0; i < root[ "ignore" ] . getLength( ); ++i ) this -> ignore . push_back( root[ "ignore" ][ i ] );
	} catch ( const lc::ParseException &e ) { spdlog::error( "Error in config file: {}, {}, {}", e . what( ), e . getFile( ), e . getLine( ) ); }

	m_hash = hash_value( *this );
}

void help::CConfig::save( fs::path path )
{
	lc::Config cfg;
	auto &     root                   = cfg . getRoot( );
	root[ "global" ][ "source" ]      = this -> source . string( );
	root[ "global" ][ "destination" ] = this -> destination . string( );

	for ( auto &i : this -> special )
	{
		auto &pair            = root[ "paths" ] . add( lc::Setting::TypeGroup );
		pair[ "pattern" ]     = i . first;
		pair[ "destination" ] = i . second;
	}

	for ( auto &i : this -> ignore ) root[ "ignore" ] . add( lc::Setting::TypeString ) = i;

	cfg . writeFile( path . string( ) );
}
