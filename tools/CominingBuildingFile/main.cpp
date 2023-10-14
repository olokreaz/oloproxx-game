#include <csignal>
#include <filesystem>

#include <unordered_map>
#include <vector>

#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

#include "utils.hpp"

import HandlersImpl;

namespace fs = std::filesystem;
namespace ux = ftxui;

using namespace fmt::literals;
using namespace std::literals;

static inline help::Config g_config;

void ExitHandler( int ) { spdlog::info( "Exit App" ); }

int main( int, char ** )
{
	using std::jthread;

	signal( SIGTERM, ExitHandler );
	signal( SIGINT, ExitHandler );

	g_config . load( "config.local" );

	efsw::FileWatcher                fw;
	std::shared_ptr< help::CWorker > worker;

	while ( true ) if ( worker ) worker -> stop( );

	std::string source = g_config . source . string( );

	spdlog::set_level( spdlog::level::trace );

	fmt::println( "{:~^100}", "+" );
	fmt::println( "{:~^100}", fmt::format( " Нажмите {} что бы закрыть приложение ", fmt::styled( "Ctrl + C", fmt::emphasis::bold ) ) );
	fmt::println( "{:~^100}", "+" );
	fmt::println( "" );

	return 0;
}
