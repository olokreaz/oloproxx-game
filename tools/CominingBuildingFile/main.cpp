#include <csignal>
#include <filesystem>

#include <unordered_map>
#include <vector>

#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/color.h>
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
static std::stop_source    g_StopSource;

void ExitHandler( int )
{
	spdlog::info( "Exit App" );
	g_StopSource . request_stop( );
}

int main( int, char ** )
{
	using std::jthread;

	signal( SIGTERM, ExitHandler );
	signal( SIGINT, ExitHandler );

	g_config . load( "config.local" );

	efsw::FileWatcher              fw;
	help::Handler< BinaryHandler > handler { g_config . ignore };

	std::string source = g_config . source . string( );

	spdlog::set_level( spdlog::level::trace );

	jthread t(
		[&fw, &handler,&source]
( std::stop_token token )
		{
			try
			{
				auto wId = fw . addWatch( ( g_config . source ) . string( ), &handler, true );
				fw . watch( );

				spdlog::info( "Start watch: {} : {}", g_config . source . string( ), ( int ) wId );

				while ( !token . stop_requested( ) ) std::this_thread::sleep_for( 1s );
			} catch ( const std::exception &ex ) { spdlog::error( "{}", ex . what( ) ); }
		}, g_StopSource . get_token( )
		);

	fmt::println( "{:~^100}", "+" );
	fmt::println( "{:~^100}", " Нажмите {} что бы закрыть приложение ", fmt::styled( "Ctrl + C", fmt::emphasis::bold  ) );
	fmt::println( "{:~^100}", "+" );
	fmt::println( "" );

	{
		ux::Element dom = ux::hbox(
					{
							ux::text( L"Слево" ) | ux::border,
							ux::text( L"Центр" ) | ux::border | ux::flex | ux::center,
							ux::text( L"Справо" ) | ux::border,
					}
					);
		auto screen = ux::Screen::Create( ux::Dimension::Full( ), ux::Dimension::Fit( dom ) );
		ux::Render( screen, dom );

		std::this_thread::sleep_for( 500ms );

		screen . Print( );
	}

	t . join( );

	return 0;
}
