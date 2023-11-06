﻿module;

#include <filesystem>
#include <zip.h>

#include <sago/platform_folders.h>

export module app.config;

#define Declarate( name, value ) constexpr auto k##name = value;
#define Declare_C( name, value) const auto k##name = value;

export namespace config { namespace error_code
	{
		#include <cerrno>
		Declarate( steam_init, 0xff000 )
		Declarate( shader, 0xff001 )
		Declarate( compile_shader, 0xff002 )
	}

	Declarate( Config_Logger_Dir, "logs" )
	Declarate( Global_logger_name, "Global" )
	Declarate( Logger_pattern, "[ %Y:%m:%d - %H:%M:%S:%F ] [ pid %P ] [ thread %t ] [ %^==== %l ====%$ ] <%n> %v" )

	Declare_C( Appdata, std::filesystem::path( sago::getDataHome( ) ) / "oloprox" );
	Declare_C( Config_file_name, kAppdata / "config.local" );
}
