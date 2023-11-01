module;

#include <filesystem>
#include <zip.h>

#include <sago/platform_folders.h>

export module config;

#define INIT( name, value ) constexpr auto k##name = value;
#define INITC( name, value) const auto k##name = value;

namespace config
{
	export namespace error_code
	{
		#include <cerrno>
		INIT( steam_init, 0xff000 )
		INIT( shader, 0xff001 )
		INIT( compile_shader, 0xff002 )
	}

	INIT( Config_Logger_Dir, "logs" )
	INIT( Global_logger_name, "Global" )
	INIT( Logger_pattern, "[ %Y:%m:%d - %H:%M:%S:%F ] [ pid %P ] [ thread %t ] [ %^==== %l ====%$ ] <%n> %v" )

	INITC( Appdata, std::filesystem::path( sago::getDataHome( ) ) / "oloprox" );
	INITC( Config_file_name, kAppdata / "config.local" );
}

#undef INIT
#undef INITC
