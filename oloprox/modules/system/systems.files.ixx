module;

#include <filesystem>
#include <vector>
#include <boost/process.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

#include <spdlog/spdlog.h>

export module systems.files;
import types;

namespace systems::files
{
	export [[nodiscard]] std::vector< uint8 > readFile( const std::filesystem::path &path )
	{
		boost::iostreams::mapped_file_source file;
		file . open( path . string( ) );

		if ( file . is_open( ) )
		{
			std::string ctx( file . data( ), file . size( ) );
			file . close( );
			std::vector< uint8 > result( file . data( ), file . data( ) + file . size( ) );
			spdlog::debug( "loeaded file: {}", path . string( ) );
			return result;
		}
		spdlog::warn( "Error opening file" );
		return { };
	}
}
