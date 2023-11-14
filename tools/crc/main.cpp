#include <filesystem>
#include <initializer_list>
#include <sstream>
#include <string>

#include <process.hpp>
#include <yaml-cpp/yaml.h>

#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

#include <glob/glob.hpp>

#include <steam/steamtypes.h>

namespace fs = std::filesystem;

class CSetting
{
public:
	fs::path                   source;
	fs::path                   dest;
	std::vector< std::string > vItems;

	CSetting( ) = default;

	CSetting( std::initializer_list< std::string > list )
	{
		uint8       count = 0;
		std::string temp;
		for ( auto &el : list )
		{
			if ( count == 0 ) this -> source = el;
			else if ( count == 1 ) this -> dest = el;
			else vItems . push_back( el );
			++count;
		}
	}
};

template< > struct YAML::convert< CSetting >
{
	static bool decode( const Node node, CSetting &settings )
	{
		settings . source = node[ "src" ] . as< std::string >( );
		settings . dest   = node[ "dest" ] . as< std::string >( );

		if ( !node[ "items" ] ) return true;
		for ( auto el : node[ "items" ] ) settings . vItems . push_back( el . as< std::string >( ) );

		return true;
	}

	static Node encode( const CSetting settings )
	{
		Node node;
		node[ "src" ]  = settings . source . string( );
		node[ "dest" ] = settings . dest . string( );

		for ( auto &el : settings . vItems ) node[ "items" ] . push_back( el );

		return node;
	}
};

class Controller
{
	std::vector< CSetting > m_vSettings;

	std::vector< std::string > m_vCmd;

public:
	Controller( )
	{
		YAML::Node node;
		if ( __argc > 1 ) node = YAML::LoadFile( __argv[ 1 ] );
		else node              = YAML::LoadFile( "config.yml" );
		for ( auto it : node ) m_vSettings . push_back( it . as< CSetting >( ) );
	}

	void build_cmd( )
	{
		for ( const auto &setting : m_vSettings )
		{
			const auto &src  = setting . source;
			const auto &dest = setting . dest;

			for ( const auto &item : setting . vItems )
			{
				const auto file = fs::path( item );
				if ( is_regular_file( file ) ) continue;
				const auto final_dest = dest / fmt::format( "data_{}_{}.h", file . filename( ), file . extension( ) . string( ) . substr( 1 ) );
			}
		}
	}
};

int wmain( int, wchar_t ** )
{
	Controller controller;

	return 0;
}
