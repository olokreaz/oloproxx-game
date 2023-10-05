#include <csignal>
#include <filesystem>

#include <unordered_map>
#include <vector>

#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include "utils.hpp"

namespace fs = std::filesystem;

using namespace fmt::literals;
using namespace std::literals;

help::Config g_config;
static bool  g_Quit;

class BinaryHandler : public help::IObserver
{
protected:
	void _onRemove( const fs::path &path ) override
	{
		const auto relative = fs::relative( path, g_config . source );
		auto       absolete = g_config . destination / relative;

		std::error_code code;
		fs::remove( absolete, code );

		if ( code ) spdlog::error( "Error removed, code error: {}, {}", code . value( ), code . message( ) );
	}

	bool _filter( fs::path &path ) override { return path . string( ) . find( ( g_config . source / "bin" ) . string( ) ) == std::string::npos; }

	void _update_context( fs::path &path ) override { }
};

void ExitHandler( int c )
{
	spdlog::info( "Exit App" );
	g_Quit = true;
}

int main( int, char ** )
{
	signal( SIGTERM, ExitHandler );
	signal( SIGINT, ExitHandler );

	try
	{
		efsw::FileWatcher              fw;
		help::Handler< BinaryHandler > handler;

		spdlog::set_level( spdlog::level::trace );

		auto wId = fw . addWatch( ( g_config . source ) . string( ), &handler, true );
		fw . watch( );

		while ( !g_Quit ) std::this_thread::sleep_for( 1s );
	} catch ( const std::exception &ex ) { spdlog::error( "{}", ex . what( ) ); }

	return 0;
}
