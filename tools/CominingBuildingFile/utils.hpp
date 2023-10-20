#pragma once
#include <string_view>
#include <spdlog/spdlog.h>

namespace utils
{
	inline std::shared_ptr< spdlog::logger > create_logger( std::string name )
	{
		return std::make_shared< spdlog::logger >( name, spdlog::default_logger( ) -> sinks( ) . begin( ), spdlog::default_logger( ) -> sinks( ) . end( ) );
	}
}
