#include <filesystem>
#include <vector>
#include <yaml-cpp/yaml.h>
#include "utils.hpp"

namespace fs = std::filesystem;

namespace YAML
{
	template< > struct convert< help::PathFormTo >
	{
		static Node encode( const help::PathFormTo &rhs )
		{
			Node node;
			node[ "src" ] = rhs . src . string( );
			node[ "dst" ] = rhs . dst . string( );
			return node;
		}

		static bool decode( const Node &node, help::PathFormTo &rhs )
		{
			if ( !node . IsMap( ) || !node[ "src" ] || !node[ "dst" ] ) return false;

			rhs . src = node[ "src" ] . as< std::string >( );
			rhs . dst = node[ "dst" ] . as< std::string >( );
			return true;
		}
	};

	template< > struct convert< std::vector< help::PathFormTo > >
	{
		static Node encode( const std::vector< help::PathFormTo > &rhs )
		{
			Node node;
			for ( auto &i : rhs ) node . push_back( i );
			return node;
		}

		static bool decode( const Node &node, std::vector< help::PathFormTo > &rhs )
		{
			if ( !node . IsSequence( ) ) return false;

			rhs . clear( );
			rhs . reserve( node . size( ) );

			for ( auto &i : node ) rhs . push_back( i . as< help::PathFormTo >( ) );
			return true;
		}
	};
}
