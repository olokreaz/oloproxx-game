module;
#include <filesystem>
#include <string>

#include <sago/platform_folders.h>

#define Declarate( name, value ) constexpr auto k##name = value;
#define Declare_C( name, value) const auto k##name = value;

export module app.commiting.config;

namespace fs = std::filesystem;
using std::string, std::string_view;

export namespace config
{
	constexpr auto kLogs_dir           = "logs";
	constexpr auto kGlobal_logger_name = "global";
	constexpr auto kLogger_pattern     = "[ %Y:%m:%d - %H:%M:%S:%F ] [ pid %P ] [ thread %t ] [ %^==== %l ====%$ ] <%n> %v";
	constexpr auto kConfig_file_name   = "config.local";
	const fs::path kAppdata            = std::filesystem::path( sago::getDataHome( ) ) / "oloprox" / "tools-sync";
}
