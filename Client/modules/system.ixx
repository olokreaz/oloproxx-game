module;

#define NODISCARD [[nodiscard]]
#define EXPORT export NODISCARD
#include <filesystem>
#include <fstream>
#include <source_location>
#include <vector>
#include <windows.h>

#include <GLFW/glfw3native.h>
#include <GLFW/glfw3native.h>

#include <fmt/core.h>
#include <fmt/format.h>
#include <spdlog/pattern_formatter-inl.h>
#include <spdlog/spdlog.h>
export module system;

namespace sys {
	EXPORT std::vector< uint8_t > readFile( const std::filesystem::path &path ) {
		std::ifstream ifs( path, std::ios::ate | std::ios::binary );

		if ( !ifs . is_open( ) ) throw std::runtime_error( "failed to open " + path . string( ) );

		std::streamsize        fileSize = ifs . tellg( );
		std::vector< uint8_t > buffer( fileSize );

		ifs . seekg( 0 );
		ifs . read( reinterpret_cast< char * >( buffer . data( ) ), fileSize );
		ifs . close( );

		return buffer;
	}

	enum status_console {
		eHide = 0 ,
		eShow ,
	};
	export struct Console {
		static void INIT( ) {
			const std::source_location src = std::source_location::current( );
			if ( !m_hConsole ) m_hConsole = ::GetConsoleWindow( );
			else
				throw fmt::system_error( errno
							, "Multiply Init() function, last call: {}:{}"
							, src . file_name( )
							, src . function_name( )
							);
		}

		static void Show( ) {
			::ShowWindow( m_hConsole, SW_SHOW );
			spdlog::info( "Show Console" );
		}

		static VOID Show( HWND hWindow ) {
			::ShowWindow( hWindow, SW_SHOW );
			spdlog::info( "Show Window: {}", hWindow -> unused );
		}

		static void Hide( ) {
			::ShowWindow( m_hConsole, SW_HIDE );
			spdlog::info( "Hide Console" );
		}

		static void Hide( HWND hWindow ) {
			::ShowWindow( hWindow, SW_HIDE );
			spdlog::info( "Hide Window: {}", hWindow -> unused );
		}

		auto static getState( ) { return m_status; }

	private:
		static enum status_console m_status;
		inline static HWND         m_hConsole;
	};
}
