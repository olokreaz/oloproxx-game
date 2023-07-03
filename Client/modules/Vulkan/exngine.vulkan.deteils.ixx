module;
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <fmt/core.h>
#include <vulkan/vulkan.h>

#define NODISCARD [[nodiscard]]
export module engine.vulkan.deteils;
import types;

namespace utils
{
	export NODISCARD std::vector< uint8_t > readFile( const std::string &path )
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

	export NODISCARD VkShaderModule createShaderModule( const std::vector< uint8 > &code, VkDevice device )
	{
		VkShaderModuleCreateInfo createInfo { };
		createInfo . sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo . codeSize = code . size( );
		createInfo . pCode    = reinterpret_cast< const uint32_t * >( code . data( ) );

		VkShaderModule shaderModule;
		if ( vkCreateShaderModule( device, &createInfo, nullptr, &shaderModule ) != VK_SUCCESS )
			throw
					std::runtime_error( "failed to create shader module!" );

		return shaderModule;
	}
}
