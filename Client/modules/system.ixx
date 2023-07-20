module;

#define NODISCARD [[nodiscard]]
#define EXPORT export NODISCARD
#include <filesystem>
#include <fstream>
#include <source_location>
#include <vector>
#include <windows.h>
#include <fmt/core.h>
#include <fmt/format.h>
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

	export enum class EWindowStatus {
		eHide ,
		eShow
	};

	export class Console {
		Console() = delete;
	public:
		
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

		static void setConsoleTittle( const std::string &tittle ) {
			::SetConsoleTitleA( tittle . c_str( ) );
			spdlog::info( "Set Console Tittle: {}", tittle );
		}

		static void setLogLevel( spdlog::level::level_enum level ) {
			spdlog::set_level( level );
			spdlog::info( "Set Log Level: {}", std::to_underlying( level ) );
		}

		auto static setStatus( EWindowStatus status, HWND hwnd = nullptr ) {
			int  command = ( status == EWindowStatus::eShow ) ? SW_SHOW : SW_HIDE;
			HWND window  = hwnd ? hwnd : m_hConsole;

			::ShowWindow( window, command );

			if ( window == m_hConsole ) {
				m_status = status;
				spdlog::info( "{} Console", status == EWindowStatus::eShow ? "Show" : "Hide" );
			} else spdlog::info( "{} Window: {}", status == EWindowStatus::eShow ? "Show" : "Hide", hwnd ? hwnd -> unused : 0 );
		}

		auto static getState( ) { return m_status; }

	private:
		inline static EWindowStatus m_status;
		inline static HWND   m_hConsole;
	};
}
