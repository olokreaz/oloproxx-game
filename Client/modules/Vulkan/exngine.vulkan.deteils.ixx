module;
#include <filesystem>
#include <format>
#include <fstream>
#include <string>
#include <vector>
#include <boost/dll.hpp>

#define NODISCARD [[nodiscard]]
export module vulkan.deteils;
import types;

export namespace deteils
{
	NODISCARD std::vector< uint8_t > readFile( const std::string &path )
	{
		std::ifstream ifs( path, std::ios::ate | std::ios::binary );

		if ( !ifs . is_open( ) ) throw std::runtime_error( "failed to open " + path );

		std::streamsize        fileSize = static_cast< std::streamsize >( ifs . tellg( ) );
		std::vector< uint8_t > buffer( fileSize );

		ifs . seekg( 0 );
		ifs . read( reinterpret_cast< char * >( buffer . data( ) ), fileSize );
		ifs . close( );

		return buffer;
	}
}
