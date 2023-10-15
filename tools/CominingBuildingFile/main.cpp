#include <csignal>
#include <filesystem>

#include <unordered_map>
#include <vector>

#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>

#include <spdlog/spdlog.h>

#include <ftxui/component/component.hpp>
#include <ftxui/component/receiver.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>

#include "utils.hpp"

namespace fs = std::filesystem;

using namespace fmt::literals;
using namespace std::literals;

using namespace ftxui;

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
	std::signal( SIGTERM, ExitHandler );
	std::signal( SIGINT, ExitHandler );

	g_config . load( "config.local" );

	spdlog::set_level( spdlog::level::trace );

	spdlog::set_pattern( "[ %Y:%m:%d - %H:%M:%S:%F ] [ %l ] [ %t ] <%n> %v" );

	auto screen = ScreenInteractive::TerminalOutput( );

	// Данные
	std::wstring                sourcePath        = L"sourcePath";
	std::vector< std::wstring > sourcePathContent = ListDirectory( sourcePath );
	std::wstring                destPath          = L"destPath";

	// Создаем компоненты
	auto sourceInput = Input(  , L"Source", InputOption::Default(  ) );
	auto sourceList  = Container::Vertical( { } );
	for ( const auto &entry : sourcePathContent )
	{
		auto cb = Checkbox( entry, nullptr, CheckboxOption::Simple( ) );
		sourceList -> Add( Container::Horizontal( {cb} ) );
	}
	auto destInput = Input( &destPath, L"destPath" );
	auto copyBtn   = Button( L"Copy", [&]( ) { CopyDirectory( sourcePath, destPath ); } );

	auto sourceContainer = Container::Vertical( { sourceInput, sourceList } );
	auto destContainer   = Container::Vertical( { destInput, copyBtn } );

	// Укладываем все вместе
	auto layout = Container::Horizontal(
					{
							sourceContainer,
							separator( ),
							destContainer,
					}
					);

	// Создаем рендерер
	auto render = Renderer(
				layout, [&]( )
				{
					return vbox(
						{
								vbox(
									{
											text( L"Source directory:" ),
											sourceInput -> Render( ),
											separator( ),
											sourceList -> Render( ) | frame | flex_grow,
									}
								) | border,
								separator( ),
								vbox(
									{
											text( L"Destination directory:" ),
											destInput -> Render( ),
											separator( ),
											hcenter( copyBtn -> Render( ) )
									}
								) | border
						}
						);
				}
				);

	screen . Loop( render );

	return 0;
}
