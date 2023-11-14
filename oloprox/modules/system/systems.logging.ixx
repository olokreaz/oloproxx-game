module;
#include <filesystem>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

export module systems.logging;
import app.config;

namespace systems::logging
{
	export std::shared_ptr< spdlog::logger > create_logger( std::string name )
	{
		static auto siDialy = std::make_shared< spdlog::sinks::daily_file_sink_mt >(
											( std::filesystem::current_path( ) / config::kConfig_Logger_Dir / "log.txt" ) .
											string( ), 0, 0
											);
		static auto siStdout = std::make_shared< spdlog::sinks::stdout_color_sink_mt >( );
		static bool bInit    = false;

		if ( !bInit )
		{
			siStdout -> set_color( spdlog::level::trace, FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE );
			bInit = true;
		}

		auto logger = std::make_shared< spdlog::logger >( name );

		logger -> sinks( ) . clear( );

		logger -> sinks( ) . push_back( siStdout );
		logger -> sinks( ) . push_back( siDialy );

		logger -> flush_on( spdlog::level::warn );
		logger -> set_pattern( config::kLogger_pattern );
		logger -> set_level( spdlog::get_level( ) );

		logger -> trace( "logger created" );

		return logger;
	}
}
