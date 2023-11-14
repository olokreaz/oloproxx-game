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

	if ( !exists( this -> m_config_path ) )
	{
		spdlog::warn( "Config file not found, creating new" );
		this -> save( );
		return;
	}

	ryml::Tree root;

	{
		root = ryml::parse_in_arena( ryml::to_csubstr( this -> m_config_path . string( ) ) );
		std::string temp;
		root[ "source" ] >> temp;
		this -> source = temp;
		root[ "destination" ] >> temp;
		this -> destination = temp;
	}
	{
		ryml::ConstNodeRef special = root[ "special" ];
		for ( auto v : special )
		{
			CSpecial temp;
			v[ "pattern" ] >> temp . pattern;
			v[ "destination" ] >> temp . destination;
			if ( v . has_child( "release" ) )
			{
				bool bRelease;
				v[ "release" ] >> bRelease;
				temp . bRelease = bRelease;
			} else temp . bRelease = std::nullopt;
			this -> special[ v . key( ) . str ] = temp;
		}
	}
	{
		std::string        temp;
		ryml::ConstNodeRef ignore = root[ "ignore" ];
		for ( auto v : ignore )
		{
			v >> temp;
			this -> ignore . push_back( temp );
		}
	}

	m_hash = hash_value( *this );
}

void help::CConfig::save( fs::path path )
{
	if ( hash_value( *this ) != m_hash ) return;
	ryml::Tree root;
	root[ "source" ] << this -> source . string( );
	root[ "destination" ] << this -> destination . string( );

	{
		ryml::NodeRef special_tree = root[ "special" ];
		for ( auto &[ _, v ] : this -> special )
		{
			// Assuming that v is a tuple and you can use structured bindings to bind its parts to pattern, destination, bRelease:
			auto &[ pattern, destination, bRelease ] = v;

			// add a map to the sequence
			auto &map = special_tree . append_child( ) << ryml::MAP;

			// populate the map with key-value pairs
			map << ryml::key( "pattern" ) << pattern;
			map << ryml::key( "destination" ) << destination;
			if ( bRelease ) map << ryml::key( "bRelease" ) << bRelease;
		}
	}
	{
		ryml::NodeRef ignore_tree = root[ "ignore" ];
		for ( auto &v : this -> ignore )
		{
			ignore_tree . append_child( ) << v;
		}
	}

	/* TODO */
}
