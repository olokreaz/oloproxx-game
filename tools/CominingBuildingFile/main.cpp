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

// Заглушка для функции копирования
void CopyDirectory( const std::wstring &source, const std::wstring &destination )
{
	// Здесь должен быть ваш код копирования директории
}

std::vector< std::wstring > ListDirectory( const std::wstring &path )
{
	// Здесь должен быть ваш код для получения списка директорий
	// Возвращаем некоторые тестовые данные для демонстрации
	return { L"item1", L"item2", L"item3", L"item4" };
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

	spdlog::flush_every( 5s );
	spdlog::flush_on( spdlog::level::level_enum::warn );

	spdlog::set_level( spdlog::level::trace );

	spdlog::set_pattern( "[ %Y:%m:%d - %H:%M:%S:%F ] [ %l ] [ %t ] <%n> %v" );

	auto screen = ux::ScreenInteractive::TerminalOutput( );

	int                        slected_left_menu_info = 0;
	std::vector< std::string > left_menu_keys         = ( g_config . specific_path | std::views::keys ) | std::ranges::to< std::vector< std::string > >( );
	auto                       left_menu_info         = Menu( &left_menu_keys, &slected_left_menu_info, ux::MenuOption::VerticalAnimated( ) );

	int                        slected_right_menu_info = 0;
	std::vector< std::string > right_menu_keys         = ( g_config . specific_path | std::views::values ) | std::ranges::to< std::vector< std::string > >( );
	auto                       right_menu_info         = Menu(
								&right_menu_keys, &slected_right_menu_info, ux::MenuOption(
															{ .on_change = [&] { spdlog::info( "right_menu_selected: {}", right_menu_keys[ slected_right_menu_info ] ); },
															.on_enter = [&]
															{
																spdlog::info(
																		"Called select {}",
																		right_menu_keys[
																			slected_right_menu_info ]
																	);
															} }
															)
								);

	auto left_phs = ux::Container::Vertical(
						{ Renderer(
							[&left_menu_info]( ) -> Element
							{
								return vbox(
									{
											text( L"source" ) | center | color( Color::HotPink3 ),
											ux::separator( ) | color( Color::HotPink3 ),
											left_menu_info -> Render( ) | color( Color::HotPink3 )
									}
									) | flex;
							}
							) }
						);
	auto right_phs = ux::Container::Vertical(
						{ Renderer(
							[&right_menu_info]( )-> Element
							{
								return vbox(
									{
											text( L"destination" ) | center | color( Color::HotPink3 ),
											ux::separator( ) | color( Color::HotPink3 ),
											right_menu_info -> Render( ) | color( Color::HotPink3 )
									}
									) | flex;
							}
							) }
						);

	auto info_group = ux::Container::Horizontal(
						{
								ui::window( L"Left Paths", left_phs ),
								ux::Renderer( [] { return ux::separator( ); } ),
								ui::window( L"Right Paths", right_phs ),
						}
						);

	auto window = ui::window( L"oloprox", ux::Container::Vertical( { info_group } ) );

	screen . Loop( window );

	return 0;
}
