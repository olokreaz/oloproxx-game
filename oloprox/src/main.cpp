#include <filesystem>

#include <sago/platform_folders.h>
#include <spdlog/spdlog.h>

import system;
import types;

import core.handler.url;

inline static const std::filesystem::path kAppDataPath = std::filesystem::path( sago::getDataHome( ) ) / ".oloprox";

int main( int, char ** ) {
	sys::Console::INIT( );
	sys::Console::setLogLevel( spdlog::level::trace );

	return 0;
}
