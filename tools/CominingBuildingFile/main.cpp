#include <csignal>
#include <filesystem>

#include <ranges>
#include <unordered_map>
#include <vector>

#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/xchar.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/daily_file_sink.h>

#include <ftxui/component/component.hpp>
#include <ftxui/component/receiver.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

#include "utils.hpp"

namespace fs = std::filesystem;

using namespace fmt::literals;
using namespace std::literals;

import pages;

static inline help::Config g_config;

static inline std::stop_source g_stop_source;

void ExitHandler( int )
{
	g_stop_source . request_stop( );
	std::this_thread::sleep_for( 2s );
	spdlog::info( "call stop token" );
}

int main( int, char ** )
{
	using namespace ftxui;
	std::signal( SIGTERM, ExitHandler );
	std::signal( SIGINT, ExitHandler );

	g_config . load( "config.local" );

	auto sink = std::make_shared< spdlog::sinks::daily_file_sink_mt >( "logs/log.txt", 0, 0 );

	auto logger = std::make_shared< spdlog::logger >( "global", sink );

	spdlog::set_default_logger( logger );

	spdlog::flush_every( 1s );
	spdlog::flush_on( spdlog::level::level_enum::warn );

	spdlog::set_level( spdlog::level::trace );

	spdlog::set_pattern( "[ %Y:%m:%d - %H:%M:%S:%F ] [ %l ] [ %t ] <%n> %v" );

	auto screen = ux::ScreenInteractive::Fullscreen( );

	bool show = false;
	auto btn  = ux::Button( L"Start", [&show] { show = !show; }, ux::ButtonOption::Animated( Color::Black, Color::HotPink2, Color::Black, Color::DeepSkyBlue1 ) );

	auto modl = ux::Modal( Renderer( [] { return text( L"\"Ctrl+C\" что-бы выйти из приложения" ) | center | borderDouble; } ), &show );

	btn = Renderer(
			btn, [&btn]
			{
				return vbox
						(
						{
								btn -> Render( )
						}
						) | color( LinearGradient( ) . Angle( 25 ) . Stop( Color::DeepPink2 ) . Stop( Color::DeepSkyBlue2 ) );
			}
			);

	auto txt = Renderer( [] { return text( L"oloprox" ) | bold | color( Color::HotPink ) | center; } );

	auto Layout = ux::Container::Vertical( { btn, txt | modl } );

	auto window = ui::window( L"oloprox", Layout );

	screen . Loop( window );

	return 0;
}
