#include <app/Configer.hpp>

#include <filesystem>
#include <fstream>
#include <vector>

#include <fmt/std.h>
#include <spdlog/spdlog.h>

import tools_sync_file.config;

void help::CConfig::load( fs::path path )
{
	this -> m_config_path = config::kAppdata / path;

	if ( !exists( this -> m_config_path ) )
	{
		spdlog::warn( "Config file not found, creating new" );
		this -> save( );
		return;
	}
	try
	{
		yml::Node root;

		{
			root                = yml::LoadFile( m_config_path . string( ) );
			this -> source      = root[ "source" ] . as< std::string >( );
			this -> destination = root[ "destination" ] . as< std::string >( );
		}
		{
			auto special = root[ "special" ];
			for ( auto v : special )
			{
				CSpecial temp;
				temp . pattern     = v[ "pattern" ] . as< std::string >( );
				temp . destination = v[ "destination" ] . as< std::string >( );
				if ( v[ "release" ] )
				{
					bool bRelease;
					bRelease        = v[ "release" ] . as< bool >( );
					temp . bRelease = bRelease;
					if ( temp . bRelease != this -> bReleased ) continue;
				} else temp . bRelease = std::nullopt;
				this -> special[ temp . pattern ] = temp;
			}
		}
		{
			std::string temp;
			yml::Node   ignores = root[ "ignore" ];
			for ( auto v : ignores )
			{
				temp = v . as< std::string >( );
				if ( !temp . empty( ) ) this -> ignore . push_back( temp );
			}
		}
	} catch ( const yml::Exception &e )
	{
		spdlog::critical( "Filed Parsing Config: {}, col: {}, line: {}, pos: {}", e . msg, e . mark . column, e . mark . line, e . mark . pos );
		exit( -1 );
	}
	m_hash = hash_value( *this );
}

void help::CConfig::save( fs::path path )
{
	if ( hash_value( *this ) != m_hash ) return;

	yml::Node root;

	root[ "source" ]      = this -> source . string( );
	root[ "destination" ] = this -> destination . string( );

	yml::Node special_tree;

	for ( auto &[ _, v ] : this -> special )
	{
		auto &    [ pattern, destination, boptRelease ] = v;
		yml::Node temp;
		temp[ "pattern" ]     = pattern;
		temp[ "destination" ] = destination;
		if ( boptRelease ) temp[ "release" ] = *boptRelease;
		special_tree . push_back( temp );
	}

	root[ "special" ] = special_tree;

	yml::Node ignore_tree;
	for ( auto &v : this -> ignore ) ignore_tree . push_back( v );

	root[ "ignore" ] = ignore_tree;

	std::ofstream( path ) << root;
}
